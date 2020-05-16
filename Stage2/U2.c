
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
    pid_t thread_id;
    thread_id = syscall(SYS_gettid);  /* Allow calls from other threads than the main one. Possible error with clang */
    ((infos_ts*) args)->thread_id = thread_id;
    ((infos_ts*) args)->pid = getpid();

    char clientfifo[64];
    sprintf(clientfifo, "/tmp/%d.%d", getpid(), thread_id);

    if (mkfifo(clientfifo, 0660) != 0) {
        printf("error: mkfifo()\n");
        exit(1);
    }
    
    int client = open(clientfifo, O_RDONLY | O_NONBLOCK);

    write(server, (infos_ts *) args, sizeof(infos_ts));
    log_maker(((infos_ts*) args)->id, ((infos_ts*) args)->pid, ((infos_ts*) args)->thread_id, ((infos_ts*) args)->dur, ((infos_ts*) args)->pos, "IWANT");

    infos_ts answer ;
    int cont; /*Attempts to read from the server before consider it's closed*/
    if(access(server_dir, F_OK) != -1){
        while (read(client, &answer, sizeof(answer)) <= 0 && cont < 5) {
            usleep(10000);  /* while no server answer */
            cont++;
        }
        if (cont >=5){
            log_maker(((infos_ts*) args)->id, ((infos_ts*) args)->pid, ((infos_ts*) args)->thread_id, ((infos_ts*) args)->dur, ((infos_ts*) args)->pos, "FAILD");
        }
        else { 
            log_maker(answer.id, getpid(), thread_id, answer.dur, answer.pos, (answer.pos != -1) ? "IAMIN" : "CLOSD");
        }
    }
    else {
        log_maker(((infos_ts*) args)->id, ((infos_ts*) args)->pid, ((infos_ts*) args)->thread_id, ((infos_ts*) args)->dur, ((infos_ts*) args)->pos, "FAILD");
    }

    close(client);

    unlink(clientfifo);

    return NULL;
}


int main(int argc, char* argv[]){
    if (argc != 4) {
        printf("--- CLIENT ---\n");
        printf("Usage: %s <-t nsecs> fifoname \n", argv[0]);
        exit(1);
    }

    client_ts client = client_handler(argv);
    server_dir = client.fifoname;

    server = -1;

    while(server == -1){
        server =  open(client.fifoname, O_WRONLY);
        if(server == -1){
            printf("Server connecting . . . \n");
            sleep(3);
        }
    }

    long int time_out = client.secs * 1000;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    int id_req = 0;

    while(time_out > time_ms()){
        pthread_t t_pid;
        infos_ts request;

        request.dur = (rand() % (20-5+1)) +5;
        request.id = id_req++;
        request.pos -=1;

        pthread_create(&t_pid, NULL, un_thrd_handler, &request);
        /*detach helps with a better paralelism by releasing the resources back to the system*/
        pthread_detach(t_pid);
        /*requests with 50ms interval*/
        usleep(50000); 
    }

    exit(0);
}
