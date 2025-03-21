#include "shared_memory.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <utility>

bool request_processed(SharedMemory* shared_memory, int index)
{
    if (shared_memory->full) {
        return false;
    }
    if (shared_memory->tail == shared_memory->head && !shared_memory->full) {
        return true;
    }

    for (int i = shared_memory->head - 1; i != shared_memory->tail; i = (i - 1) % QUEUE_SIZE) {
        if (i == index) {
            return false;
        }
    }

    return shared_memory->tail != index;
}

void send_request(
    SharedMemory* shared_memory,
    Operations type,
    std::pair<const std::string&, const std::string&> arguments)
{
    int index;

    pthread_mutex_lock(&shared_memory->mutex);

    if (shared_memory->full) {
        std::cout << "Queue is full" << '\n';
        pthread_cond_wait(&shared_memory->cond_var, &shared_memory->mutex);
        return;
    }

    index = shared_memory->head;
    Request* request = &shared_memory->request[index];
    request->type = type;
    strncpy(request->key, serialize(arguments.first).c_str(), MAX_KEY_SIZE);
    strncpy(request->value, serialize(arguments.second).c_str(), MAX_VALUE_SIZE);
    shared_memory->head = (1 + shared_memory->head) % QUEUE_SIZE;
    shared_memory->full = shared_memory->head == shared_memory->tail;
    pthread_cond_signal(&shared_memory->cond_var);

    pthread_mutex_unlock(&shared_memory->mutex);

    std::cout << "Command sent" << '\n';

    if (type == Operations::GET) {
        pthread_mutex_lock(&shared_memory->mutex);

        while (!request_processed(shared_memory, index)) {
            pthread_cond_wait(&shared_memory->cond_var, &shared_memory->mutex);
        }
        std::string result(shared_memory->request[index].response);
        pthread_mutex_unlock(&shared_memory->mutex);
        std::cout << "Result: " << result << std::endl;
    }
}

int main()
{
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cout << "Server not running" << '\n';
        return -1;
    }

    SharedMemory* shared_memory =
        (SharedMemory*)mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(3), serialize(3)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(4), serialize(5)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(6), serialize(6)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(7), serialize(3)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(8), serialize(3)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(9), serialize(3)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(10), serialize(3)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(11), serialize(3)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(12), serialize(3)));

    std::cout << "Start inserting.." << '\n';
    send_request(shared_memory, INSERT, std::pair(serialize(13), serialize(4)));

    std::cout << "Start PRINTING.." << '\n';
    send_request(shared_memory, PRINT, std::pair(serialize(0), serialize(0)));

    munmap(shared_memory, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
