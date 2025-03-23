#pragma once

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <optional>
#include <pthread.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

#include "shared_memory.h"

/**
 * @class Client
 * @brief Represents the client, which performs actions on the hashtable of the server.
 */
class Client {
public:
    /**
     * @brief Constructs a new client and opens the shared memory.
     */
    Client();

    /**
     * @brief Unmaps the shared memory.
     */
    ~Client();

    /**
     * @brief Main client program.
     *
     * @details The user can choose the operation and the arguments, which will be sent to the
     * server.
     */
    void start_client();

private:
    /**
     * @brief Memory which is shared with the server.
     */
    SharedMemory* shared_memory;

    /**
     * @brief File descriptor for the shared memory, used to unmap and close the memory at the end.
     */
    int shm_fd;

    /**
     * @brief Sends a request to the server.
     *
     * @param shared_memory The memory to use.
     * @param type The type of the operation the server has to process.
     * @param k First potential argument of the request, represents the key.
     * @param v Second potential argument of the request, represent the value.
     * @return int The index of the request in the circular-buffer, so we can access it again for
     * processing the respond.
     */
    int send_request(
        SharedMemory* shared_memory,
        Operations type,
        std::optional<const std::string> k,
        std::optional<const std::string> v);

    /**
     * @brief Determines if the request was processed by the server.
     *
     * @param shared_memory The memory to use.
     * @param index The index of the request in the circular-buffer.
     * @return bool The request was processed by the server.
     */
    bool request_processed(SharedMemory* shared_memory, int index);

    /**
     * @brief Processes the respond of the server.
     *
     * @param shared_memory The memory to use.
     * @param index The index of the request in the circular-buffer.
     * @return std::string The response of the server as a string.
     */
    std::string process_respond(SharedMemory* shared_memory, int index);
};
