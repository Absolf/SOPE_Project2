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
#include <signal.h>
#include <stdbool.h>

extern struct timespec begin;

extern int server;

/* Q1 Structures and auxiliar functions*/ 

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

server_ts server_handler(int argc, char** args);

double time_ms();

void log_maker(int id, pid_t pid, pid_t thread_id, int dur, int pos, char *op);


/* Q2 structures and auxiliar functions to the queue of spots to request/be requested */
typedef struct {
    int begin;
    int end;
    int lenght;
    int max_lenght;
    int* queue;
} spots_ts;

spots_ts new_spots(int max_lenght);

bool empty(spots_ts* spot);

bool pushable(spots_ts* spots, int places);

int pop(spots_ts* spots);

int filler(spots_ts * spots);


#endif