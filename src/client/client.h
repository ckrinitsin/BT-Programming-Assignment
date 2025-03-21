#pragma once

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <optional>
#include <pthread.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <utility>

#include "shared_memory.h"
class Client {
public:
    Client();
    ~Client();

    void start_client();

private:
    SharedMemory* shared_memory;
    int shm_fd;

    int send_request(
        SharedMemory* shared_memory,
        Operations type,
        std::optional<const std::string> k,
        std::optional<const std::string> v);
    bool request_processed(SharedMemory* shared_memory, int index);
    std::string process_result(SharedMemory* shared_memory, int index);
};
