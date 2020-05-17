
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>


#include "utils.h"

int server;

char *server_dir;

struct timespec begin;

void* un_thrd_handler(void * args){
    pthread_t thread_id;
    pthread_detach(thread_id = pthread_self());

    infos_ts request;
    memcpy(&request, ((infos_ts*) args), sizeof(infos_ts));

    request.thread_id = thread_id;
    request.pid = getpid();

    char clientfifo[64];
    sprintf(clientfifo, "/tmp/%d.%ld", getpid(), thread_id);

    if (mkfifo(clientfifo, 0660) != 0) {
        printf("error: mkfifo() can't create \n");
        exit(1);
    }
    write(server, (infos_ts *) args, sizeof(infos_ts));
	int client = open(clientfifo, O_RDONLY | O_NONBLOCK);
    if(client <0){
        char *message =(char*) malloc (128 * sizeof(char));
        sprintf(message, "Can't read private FIFO %d ", request.id);
        perror(message);
        free(message);
        log_maker(request.id, request.pid, request.thread_id, request.dur, request.pos, "FAILD");

        if (unlink(clientfifo) < 0)
            perror("Private FIFO can't be unlinked");
        return NULL;
    }

    
    log_maker(request.id, request.pid, request.thread_id, request.dur, request.pos, "IWANT");

    /*  15 Attempts to get an answer from the server  */
    infos_ts answer ;
    int cont; 
    while (read(client, &answer, sizeof(infos_ts)) <= 0 && cont < 15) {
		usleep(1000);  /* while no server answer */
        cont++;
    }
	if (cont >=15){
		log_maker(request.id, request.pid, request.thread_id, request.dur, request.pos, "FAILD");
		close(client);
		unlink(clientfifo);
        if (unlink(clientfifo) < 0)
            perror("Error unlinking client fifo");
        return NULL;
    }
    
	log_maker(answer.id, getpid(), thread_id, answer.dur, answer.pos, (answer.pos != -1) ? "IAMIN" : "CLOSD");

    if (unlink(clientfifo) < 0)
        perror("Error unlinking client fifo");

    close(client);
    return NULL;
}


int main(int argc, char* argv[]){
    if (argc != 4) {
        printf("--- CLIENT ---\n");
        printf("Usage: %s [<-t nsecs] [fifoname] \n", argv[0]);
        exit(1);
    }

    client_ts client = client_handler(argv);

    server = -1;

    while(server == -1){
        server =  open(client.fifoname, O_WRONLY);
        if(server == -1){
            printf("Server connecting . . . \n");
            sleep(1);
        }
    }

    long int time_out = client.secs * 1000;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    int id_req = 0;

    while(time_out > time_ms()){
        pthread_t t_pid;
        infos_ts request;
        
        // the client request time for a wc will be between 15 and  90 ms
        request.dur = (rand() % (90 -15 +1)) + 15;
        request.id = id_req++;
        request.pos -=1;

        pthread_create(&t_pid, NULL, un_thrd_handler, &request);
        /*detach helps with a better paralelism by releasing the resources back to the system*/
        //pthread_detach(t_pid);
        /*requests with 50ms interval*/
        usleep(50000); 
    }

    exit(0);
}
