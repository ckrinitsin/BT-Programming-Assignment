#pragma once

#include <pthread.h>

#define QUEUE_SIZE 10
#define SHM_NAME "/hashtable_queue"

enum Operations { INSERT, DELETE, GET };

struct SharedMemory {
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
    Operations queue[QUEUE_SIZE];
    int head;
    int tail;
};
