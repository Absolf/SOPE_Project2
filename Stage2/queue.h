
#ifndef QUEUES_H
#define QUEUES_H

#include <stdlib.h>
#include <stdbool.h>


/* Q2 structures and auxiliar functions to the queue of spots to request/be requested */
typedef struct {
    int begin;
    int end;
    int lenght;
    int max_lenght;
    int* queue;
} spots_ts;

spots_ts new_spots(int max_lenght){
    spots_ts spots;
    spots.lenght = 0;
    spots.max_lenght = max_lenght;
    spots.end = max_lenght - 1;
    spots.begin = 0;
    spots.queue = (int*) malloc(max_lenght * sizeof(int));
    return spots;
}

int empty(spots_ts* spot){
    return spot->lenght == 0;
}

bool pushable(spots_ts* spots, int places){
    if(!(spots->lenght == spots->max_lenght)){
        spots->end = (spots->end +1) % spots->max_lenght;
        spots->queue[spots->end] = places;
        spots->lenght++;
        return false;
    }
    return true;
}

int pop_front(spots_ts* spots){
    int aux = -1;
    if(!empty(spots)){
        aux = spots->queue[spots->begin];
        spots->begin = (spots->begin + 1)% spots->max_lenght;
        spots->lenght--;
    }  
    return aux;
}

int filler(spots_ts * spots){
    for(int i = 1; i <= spots->max_lenght; i++){
        if (pushable(spots, i)){
            return 1;
        }    
    }
    return 0;
}

#endif //QUEUES_H
