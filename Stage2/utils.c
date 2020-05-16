#include "utils.h"


client_ts client_handler(char **args) {
    client_ts handler;
    handler.secs = atoi(args[2]);
    handler.fifoname = args[3];
    return handler;
}

server_ts server_handler(int argc, char **args) {
    server_ts handler;
    handler.secs = atoi(args[2]);
    handler.places = 0;
    handler.threads = 0;
    handler.fifoname = args[3];

    int aux = 3;
    while(aux < argc-1){
        if(strcmp("-l", args[aux]) == 0)
            handler.places = atoi(args[aux + 1]);
        else if(strcmp("-n", args[aux]) == 0)
            handler.threads = atoi(args[aux + 1]);
        aux++;   
    }
    handler.fifoname = args[argc-1];

    return handler;
}

double time_ms() {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    return (end.tv_sec - begin.tv_sec) * 1000.0 + (end.tv_nsec - begin.tv_nsec) / 1000000.0;
}

void log_maker(int id, pid_t pid, pid_t thread_id, int dur, int pos, char *op) {
    char* msg;
    msg = (char*) malloc (128 * sizeof(char));
    time_t timing = time(NULL);

    sprintf(msg, "%ld ; %d ; %d ; %d ; %d ; %d ; %s\n", timing, id, pid, thread_id, dur, pos, op);
    write(STDOUT_FILENO, msg, strlen(msg));
}

//Q2 structures and aux functions to the queue

spots_ts new_spots(int max_lenght){
    spots_ts spots;
    spots.lenght = 0;
    spots.max_lenght = max_lenght;
    spots.end = max_lenght--;
    spots.begin = 0;
    spots.queue = (int*) malloc(max_lenght * sizeof(int));
    return spots;
}

bool empty(spots_ts* spot){
    if (spot->lenght == 0){
        return true;
    }
    return false;
}

bool pushable(spots_ts* spots, int places){
    if((spots->lenght == spots->max_lenght)){
        return false;
    }
    spots->end = (spots->end +1) % spots->max_lenght;
    spots->queue[spots->end] = places;
    spots->lenght += 1;
    return true;
}

int pop(spots_ts* spots){
    int aux = 0;
    if(!empty(spots)){
        aux = spots->queue[spots->begin];
        spots->begin = (spots->begin +1)% spots->max_lenght;
        spots->lenght -= 1 ;
    }  
    return aux;
}

int filler(spots_ts * spots){
    int i = 1;
    while(i <= spots->max_lenght){
        if(!pushable(spots, i))
            return i;
        i++;
    }
    return 0;
}




