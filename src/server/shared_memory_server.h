#pragma once

#include "hashtable.h"
#include "shared_memory.h"
#include <cstring>
#include <fcntl.h>
#include <optional>
#include <sys/mman.h>
#include <unistd.h>

template <typename K, typename V>
class SharedMemoryServer {
public:
    SharedMemoryServer(size_t size)
        : hash_table(size)
    {
        shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

        ftruncate(shm_fd, sizeof(SharedMemory));

        shared_memory = (SharedMemory*)
            mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        shared_memory->tail = shared_memory->head = 0;

        pthread_mutexattr_t mutex_attr;
        pthread_condattr_t cond_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
        pthread_condattr_init(&cond_attr);
        pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&shared_memory->mutex, &mutex_attr);
        pthread_cond_init(&shared_memory->cond_var, &cond_attr);
    }

    ~SharedMemoryServer()
    {
        munmap(shared_memory, sizeof(SharedMemory));
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }

    void process_requests()
    {
        while (true) {
            pthread_mutex_lock(&shared_memory->mutex);

            if (shared_memory->tail == shared_memory->head && !shared_memory->full) {
                pthread_cond_wait(&shared_memory->cond_var, &shared_memory->mutex);
            }

            Request* request = &shared_memory->request[shared_memory->tail];

            K key = deserialize<K>(request->key);
            V value = deserialize<V>(request->value);

            switch (request->type) {
            case INSERT:
                std::cout << "Insert operation" << '\n';
                if (hash_table.insert(key, value)) {
                    strncpy(
                        request->response,
                        serialize<std::string>("Inserted successfully").c_str(),
                        MAX_VALUE_SIZE);
                } else {
                    strncpy(
                        request->response,
                        serialize<std::string>("Key is already available").c_str(),
                        MAX_VALUE_SIZE);
                }
                break;
            case GET: {
                std::cout << "Get operation" << '\n';
                hash_table.insert(key, value);
                std::optional<V> result = hash_table.get(key);
                if (result.has_value()) {
                    std::string response = serialize<V>(result.value());
                    strncpy(request->response, response.c_str(), MAX_VALUE_SIZE);
                } else {
                    strncpy(
                        request->response,
                        serialize<std::string>("Couldn't get any value").c_str(),
                        MAX_VALUE_SIZE);
                }
                break;
            }
            case DELETE:
                std::cout << "Remove operation" << '\n';
                if (hash_table.remove(key)) {
                    strncpy(
                        request->response,
                        serialize<std::string>("Key successfully deleted").c_str(),
                        MAX_VALUE_SIZE);
                } else {
                    strncpy(
                        request->response,
                        serialize<std::string>("Couldn't find key'").c_str(),
                        MAX_VALUE_SIZE);
                }
                break;
            case PRINT:
                std::cout << "Print operation" << '\n';
                strncpy(
                    request->response,
                    serialize<std::string>(hash_table.string()).c_str(),
                    MAX_VALUE_SIZE);
                break;
            default:
                break;
            }
            shared_memory->tail = (1 + shared_memory->tail) % QUEUE_SIZE;
            shared_memory->full = false;
            pthread_cond_signal(&shared_memory->cond_var);
            pthread_mutex_unlock(&shared_memory->mutex);
        }
    }

private:
    HashTable<K, V> hash_table;

    int shm_fd;
    SharedMemory* shared_memory;
};
