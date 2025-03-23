#include <csignal>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "shared_memory_server.h"

Server<int, int> shm;

/**
 * @brief Shuts the server down, when pressing <Ctrl+C>.
 *
 * @param signal Specifies the signal, which was caught.
 */
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Server shutting down" << '\n';
        shm.terminate_server();
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <number-of-buckets>\n";
        return 1;
    }

    uint32_t size;
    try {
        size = std::stoi(std::string(argv[1]));
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid argument" << '\n';
        return 1;
    }

    shm.initialize_hashtable(size);

    std::signal(SIGINT, signal_handler);

    shm.process_requests();

    return 0;
}
