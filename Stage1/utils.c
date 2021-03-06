#include "utils.h"


client_ts client_handler(char **args) {
    client_ts handler;

    handler.secs = atoi(args[2]);
    handler.fifoname = args[3];

    return handler;
}

server_ts server_handler(char **args) {
    server_ts handler;
    handler.secs = atoi(args[2]);
    handler.places = 0;
    handler.threads = 0;
    handler.fifoname = args[3];

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




