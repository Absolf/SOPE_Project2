
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <pthread.h>
#include <semaphore.h>
#include "utils.h"


static spots_ts spots; /* queue with free spots */
static int thread_flag = 0;  /* flag that will be activated if thread numbers has a limit */
static sem_t nthreads; /* semaphore that handles active threads */
static int spots_flag = 0;  /* flag that will be activated in case the number of spots has a limit */
static sem_t nspots;   /* semaphore that handles the ammount of filled spots */

static pthread_mutex_t mut_init = PTHREAD_MUTEX_INITIALIZER;

int server;


long int time_out;

struct timespec begin;



void* qn_thrd_handler(void* args){
	pthread_t thread_id;

    pthread_detach(thread_id = pthread_self()); // auto-detach
    //thread_id = syscall(SYS_gettid);

    infos_ts request;

    memcpy(&request, ((infos_ts*) args), sizeof(infos_ts));
    //request
    log_maker(request.id, getpid(), thread_id, request.dur, request.pos, "RECVD");

    /* fifo client path */
    char fifo_path[128];
    sprintf(fifo_path, "/tmp/%d.%d", request.pid, request.thread_id);
    /*attempt to open private fifo, sending GAVUP on failure*/
    int client = open(fifo_path, O_WRONLY);

    if(client < 0){
        fprintf(stderr, "Error: attempt to open private fifo with request= %d\n", request.id);
        log_maker(request.id, getpid(), thread_id, request.dur, request.pos, "GAVUP");

        //checking thread limit
        if(thread_flag) 
            sem_post(&nthreads);
        return NULL;
    }


	/*  trying initializing the answer to the request*/
    infos_ts answer;
    answer.id = request.id;
    answer.pid = getpid();
    answer.thread_id = thread_id;
    answer.dur = request.dur;
    
    // Server will give 1 for every request in case we have infinit spots, otherwise will give the next place inside the queue

    int aux_spot = 1;
    if(spots_flag){
        sem_wait(&nspots);
        pthread_mutex_lock(&mut_init);
        aux_spot = pop(&spots);
        pthread_mutex_unlock(&mut_init);
    }
    
    //Here the server gives the propper answer
    answer.pos = aux_spot;
    int aux_answer = write(client, &answer, sizeof(infos_ts));
    //if the wanted time doesn't exceed the execution time, then go
    if (time_ms() + request.dur < time_out) {
        //can't stablish conection with the client
        if(aux_answer < 0){
            fprintf(stderr, "Error: private fifo request [%d] Accepted! \n", request.id);
            log_maker(answer.id, answer.pid, answer.thread_id, answer.dur, answer.pos, "GAVUP");
            close(client);
            //thread sync
            if(thread_flag) 
                sem_post(&nthreads);
            return NULL;
        }
        //There isn't any communication with the private fifo
        close(client);

        //Client enter in the server service, until the usage time ends
        log_maker(answer.id, getpid(), thread_id, answer.dur, answer.pos, "ENTER");
        usleep(answer.dur * 1000);
        log_maker(answer.id, getpid(), thread_id, answer.dur, answer.pos, "TIMUP");
    }
    else {
        answer.pos = -1; // This means the closure of a WC
        if(aux_answer < 0){
            fprintf(stderr, "Error: private fifo request [%d] Denied! \n", request.id);
            log_maker(answer.id, answer.pid, answer.thread_id, answer.dur, answer.pos, "GAVUP");
            close(client);
            //thread SYNC
            if (thread_flag)
                sem_post(&nthreads);
            return NULL;
        }
        log_maker(answer.id, getpid(), thread_id, answer.dur, answer.pos, "2LATE");
    }
    if(thread_flag)
        sem_post(&nthreads);
    if(spots_flag){
        pthread_mutex_lock(&mut_init);
        pushable(&spots, aux_spot);
        pthread_mutex_unlock(&mut_init);
        sem_post(&nspots);
    }
    close(client);
    return NULL;
}


int main(int argc, char** argv){
	if (argc <= 3 || argc >=7) {
        printf("--- SERVER 2--- \n");
        printf("Usage: %s [-t nsec] [-l nplaces] [-t nthreads] [fifoname] \n", argv[0]);
        exit(1);
    }

    server_ts server = server_handler(argc, argv);
    
	
    if (mkfifo(server.fifoname, 0660) != 0) {
        perror("Error: Can't create fifo  \n");
        exit(1);
    }

    int file = open(server.fifoname, O_RDONLY | O_NONBLOCK);

    time_out = server.secs * 1000;
	    
    if(server.threads)
        thread_flag = 1;
    if(server.places)
        spots_flag = 1;


    /* Thread syncronization */
    if(thread_flag){
        sem_init(&nthreads, 0 , server.threads);
    }
    if(spots_flag){
        sem_init(&nspots, 0 , server.places);
        spots = new_spots(server.places);
        filler(&spots);
    }
    /* end of syncronization */

    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    
	while (time_out > time_ms()) {
        infos_ts request;
		pthread_t t_pid;
        while (read(file, &request, sizeof(server_ts)) <= 0  && time_ms() < time_out) {
            usleep(1000);
        }

        if(time_out <= time_ms()) break;

        pthread_create(&t_pid, NULL,qn_thrd_handler , &request);
         /*detach helps with a better paralelism by releasing the resources back to the system*/
        pthread_detach(t_pid);
    }

    close(file);
    int unlinker = unlink(server.fifoname);
    if(unlinker < 0)
        perror("Error: unlinked public FIFO");

    exit(0);
}
