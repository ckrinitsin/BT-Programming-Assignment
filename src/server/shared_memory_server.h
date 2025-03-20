#pragma once

#include "hashtable.h"
#include "shared_memory.h"
#include <fcntl.h>
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

        shm = (SharedMemory*)
            mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        pthread_mutexattr_t mutex_attr;
        pthread_condattr_t cond_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
        pthread_condattr_init(&cond_attr);
        pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&shm->mutex, &mutex_attr);
        pthread_cond_init(&shm->cond_var, &cond_attr);

        shm->head = (shm->tail = 0);
    }

    ~SharedMemoryServer()
    {
        munmap(shm, sizeof(SharedMemory));
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }

    void process_requests();

private:
    HashTable<K, V> hash_table;

    int shm_fd;
    SharedMemory* shm;
};
