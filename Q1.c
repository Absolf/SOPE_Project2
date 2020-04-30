
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>

#include "utils.h"



int main(int argc, char* argv[]){

    if (argc != 4) {
        printf("--- SERVER ---\n");
        printf("Usage: %s <-t nsec>  fifoname\n", argv[0]);
        exit(1);
    }
    return 0;
}
