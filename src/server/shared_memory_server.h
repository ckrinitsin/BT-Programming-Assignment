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
        shared_memory->processed = true;
    }

    ~SharedMemoryServer()
    {
        munmap(shared_memory, sizeof(SharedMemory));
        close(shm_fd);
        shm_unlink(SHM_NAME);
        std::cout << "bye" << '\n';
    }

    void process_requests()
    {
        while (true) {
            sleep(1); // TODO: remove sleep, add a mutex for shared memory, should then work with
                      // queue size 1
            if (!shared_memory->processed) {
                std::cout << "Got a request!" << '\n';

                Request request = shared_memory->request;
                shared_memory->processed = true;

                K key = deserialize<K>(request.key);
                V value = deserialize<V>(request.value);

                switch (request.type) {
                case INSERT:
                    hash_table.insert(key, value);
                    break;
                case GET: {
                    std::optional<V> result = hash_table.get(key);
                    if (result.has_value()) {
                        std::string response = serialize<V>(result.value());
                        strncpy(shared_memory->response, response.c_str(), MAX_VALUE_SIZE);
                    }
                    break;
                }
                case DELETE:
                    hash_table.remove(key);
                    break;
                case PRINT:
                    hash_table.print();
                    break;
                default:
                    break;
                }
            }
        }
    }

private:
    HashTable<K, V> hash_table;

    int shm_fd;
    SharedMemory* shared_memory;
};
