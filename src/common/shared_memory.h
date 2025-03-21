#pragma once

#include <pthread.h>
#include <sstream>

#define SHM_NAME "/hashtable_queue"
#define QUEUE_SIZE 10

#define MAX_KEY_SIZE 64
#define MAX_VALUE_SIZE 128

enum Operations { INSERT, DELETE, GET, PRINT };

struct Request {
    Operations type;
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
    char response[MAX_VALUE_SIZE];
};

struct SharedMemory {
    Request request[QUEUE_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;

    int tail;
    int head;
    bool full;
};

template <typename T>
std::string serialize(const T& data)
{
    std::ostringstream oss;
    oss << data;
    return oss.str();
}

template <typename T>
T deserialize(const std::string& str)
{
    std::istringstream iss(str);
    T data;
    iss >> data;
    return data;
}
