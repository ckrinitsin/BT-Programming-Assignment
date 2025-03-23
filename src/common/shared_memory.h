#pragma once

#include <pthread.h>
#include <sstream>

#define SHM_NAME "/hashtable_queue"
#define QUEUE_SIZE 10

#define MAX_KEY_SIZE 64
#define MAX_VALUE_SIZE 128

/**
 * @brief Possible operations on the hashtable.
 */
enum Operations { INSERT, DELETE, GET, PRINT };

/**
 * @brief Possible statuses of a request.
 */
enum Status { FREE, SENT, PROCESSED };

/**
 * @brief One request constists out of the operation, the arguments and the response.
 */
struct Request {
    Operations type;
    Status status = FREE;
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
    char response[MAX_VALUE_SIZE];
};

/**
 * @brief The shared memory between client and server.
 *
 * @details The shared memory consists out of:
 * - A circular-buffer, to de- and enqueue multiple request at once.
 * - A mutex with a conditional variable, to ensure concurrency of the buffer.
 */
struct SharedMemory {
    Request request[QUEUE_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;

    int tail;
    int head;
};

/**
 * @brief Converts a generic type to a stringstream, so that it can be saved in the shared memory.
 */
template <typename T>
std::string serialize(const T& data)
{
    std::ostringstream oss;
    oss << data;
    return oss.str();
}

/**
 * @brief Converts a stringstream to a generic type, so that value in the shared memory can be read.
 */
template <typename T>
T deserialize(const std::string& str)
{
    std::istringstream iss(str);
    T data;
    iss >> data;
    return data;
}
