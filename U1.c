
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "utils.h"

struct timespec begin;

int main(int argc, char* argv[]){
    int server;
    if (argc != 4) {
        printf("--- CLIENT ---\n");
        printf("Usage: %s <-t nsecs> fifoname \n", argv[0]);
        exit(1);
    }

    client_ts client = client_handler(argv);

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

        request.dur = (rand() % (3-1+1)) +1;
        request.id = id_req++;
        request.pos -=1;

        pthread_create(&t_pid, NULL, un_thrd_handler(server), &request);
        usleep(50000);
    }

    exit(0);
}
