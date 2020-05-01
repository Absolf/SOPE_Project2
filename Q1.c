
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "utils.h"



int main(int argc, char* argv[]){

    if (argc != 4) {
        printf("--- SERVER ---\n");
        printf("Usage: %s <-t nsec>  fifoname\n", argv[0]);
        exit(1);
    }
	
	
    server_ts server = server_handler(argv);

    if (mkfifo(server.fifoname, /* MASK */) < 0) {
        printf("Error on creating FIFO file \n");
        exit(1);
    }
	
	
    

    long int time_out = server.secs * 1000;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
	int file = open(server.fifoname, O_RDONLY | O_NONBLOCK);
    
	while (time_out > time_ms()) {
        server_ts request;
		pthread_t t_pid;
        while (read(fd, &request, sizeof(server_ts)) <= 0) {
            usleep(50000);
        }
        pthread_create(&t_pid, NULL,/* i think its a different -> un_thrd_handler(server) */, &request);
    }

    close(file);

    exit(0);
}
