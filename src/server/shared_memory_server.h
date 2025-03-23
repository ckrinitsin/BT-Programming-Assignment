#pragma once

#include "hashtable.h"
#include "shared_memory.h"
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <optional>
#include <sys/mman.h>
#include <unistd.h>

/**
 * @class Server
 * @brief Represents the server, which performs operations on the hashtable based on the requests of
 * the client.
 */
template <typename K, typename V>
class Server {
public:
    /**
     * @brief Constructs a new hashtable and initializes a shared memory buffer.
     */
    Server()
    {
        shm_fd = shm_open(SHM_NAME, O_EXCL | O_CREAT | O_RDWR, 0666);
        if (-1 == shm_fd) {
            std::cout << "Server is already running!" << '\n';
            exit(-1);
        }

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

    /**
     * @brief Unmaps and unlinks the shared memory.
     */
    ~Server()
    {
        munmap(shared_memory, sizeof(SharedMemory));
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }

    /**
     * @brief Initializes the hashtable.
     *
     * @param size The number of buckets in the hashtable.
     */
    void initialize_hashtable(size_t size) { hash_table = HashTable<K, V>(size); }

    /**
     * @brief The main loop of the server.
     *
     * @details The server checks the shared memory for new requests and executes them.
     */
    void process_requests()
    {
        while (true) {
            pthread_mutex_lock(&shared_memory->mutex);

            if (shared_memory->request[shared_memory->head].status != SENT) {
                pthread_cond_wait(&shared_memory->cond_var, &shared_memory->mutex);
            }

            Request* request = &shared_memory->request[shared_memory->head];

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
                        serialize<std::string>("Couldn't find the key").c_str(),
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
            shared_memory->request[shared_memory->head].status = PROCESSED;
            shared_memory->head = (1 + shared_memory->head) % QUEUE_SIZE;
            pthread_cond_signal(&shared_memory->cond_var);
            pthread_mutex_unlock(&shared_memory->mutex);
        }
    }

private:
    /**
     * @brief The hashtable.
     */
    HashTable<K, V> hash_table;

    /**
     * @brief Memory which is shared with the client.
     */
    SharedMemory* shared_memory;

    /**
     * @brief File descriptor for the shared memory, used to unmap and close the memory at the end.
     */
    int shm_fd;
};
