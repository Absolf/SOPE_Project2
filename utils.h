#ifndef UTILS_H //for better includes
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/syscall.h>


extern struct timespec begin;

typedef struct {
    char *fifoname /*public server fifo channel*/;
    int secs; /* working time */
} client_ts;


typedef struct{
    char * fifoname; /*public fifo channel */
    int secs; /* time to allow */
    int places;
    int threads;
} server_ts;

typedef struct {
    int id; /* request ID -> begins with zero*/
    pid_t pid; /* actual process ID*/
    int thread_id; /*actual thread ID*/
    int dur; /*client request time to server */
    int pos; /* client position given by the server */
} infos_ts;

client_ts client_handler(char** args);

server_ts server_handler(char** args);

double time_ms();

void log_maker(int id, pid_t pid, pid_t thread_id, int dur, int pos, char *op);

void* un_thrd_handler(int server);


#endif