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

    write(server, (infos_ts *) arg, sizeof(infos_ts));
    log_maker(((infos_ts*) arg)->id, ((infos_ts*) arg)->pid, ((infos_ts*) arg)->thread_id, ((infos_ts*) arg)->dur, ((infos_ts*) arg)->pos, "IWANT");

    infos_ts answer ;
    while (read(client, &answer, sizeof(answer)) <= 0) {
        signal(SIGPIPE, SIG_IGN);
        usleep(10000);  /* while no server answer */
    }
    
    log_maker(answer.id, getpid(), thread_id, answer.dur, answer.pos, (answer.pos != -1) ? "IAMIN" : "CLOSD");

    close(client);
    unlink(clientfifo);
    return NULL;
}


void* qn_thrd_handler(int server){
	void *arg = ((infos_ts*) arg);
	pid_t thread_id;
    thread_id = syscall(SYS_gettid);

    infos_ts* request = (infos_ts*) arg;
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
	/* requesito da primeira ou segunda etapa? */
    if (/* condition yet to figure out */) {
        log_maker(request->id, getpid(), thread_id, request->dur, 1, "ENTER");

        write(client, &answer, sizeof(infos_ts));
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
