
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <pthread.h>

#include "utils.h"

int server;

long int time_out;

struct timespec begin;

void* qn_thrd_handler(void* args){
	pid_t thread_id;

    thread_id = syscall(SYS_gettid);

    infos_ts* request = (infos_ts*) args;
    log_maker(request->id, getpid(), thread_id, request->dur, request->pos, "RECVD");

    /* fifo client path */
    char fifo_path[64];
    sprintf(fifo_path, "/tmp/%d.%d", request->pid, request->thread_id);
    int client = open(fifo_path, O_WRONLY);

	/*  trying initializing like this */
    infos_ts answer;
    answer.id = request->id;
    answer.pid = getpid();
    answer.thread_id = thread_id;
    answer.dur = request->dur;

    if (time_ms() + request->dur < time_out) {
        answer.pos = 1;

        write(client, &answer, sizeof(infos_ts));

        log_maker(request->id, getpid(), thread_id, request->dur, 1, "ENTER");

        usleep(request->dur * 1000);

        log_maker(request->id, getpid(), thread_id, request->dur, 1, "TIMUP");
    }
    else {
        log_maker(request->id, getpid(), thread_id, request->dur, -1, "2LATE");
        answer.pos = -1;
        write(client, &answer, sizeof(infos_ts));
    }

    close(client);
    return NULL;
}


int main(int argc, char** argv){
	if (argc != 4) {
        printf("--- SERVER ---\n");
        printf("Usage: %s <-t nsec>  fifoname\n", argv[0]);
        exit(1);
    }
	
    server_ts server = server_handler(argv);
    
	
    if (mkfifo(server.fifoname, 0660) != 0) {
        printf("Error on mkfifo \n");
        exit(1);
    }
    int file = open(server.fifoname, O_RDONLY | O_NONBLOCK);

    time_out = server.secs * 1000;
	    
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    
	while (time_out > time_ms()) {
        infos_ts request;
		pthread_t t_pid;
        while (read(file, &request, sizeof(server_ts)) <= 0  && time_ms() < time_out) {
            usleep(50000);
        }

        if(time_out <= time_ms()) break;

        pthread_create(&t_pid, NULL,qn_thrd_handler , &request);
         /*detach helps with a better paralelism by releasing the resources back to the system*/
        pthread_detach(t_pid);
    }

    close(file);
	unlink(server.fifoname);
    exit(0);
}
