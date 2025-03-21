#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "shared_memory_server.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "One argument required" << '\n';
        return 1;
    }

    uint32_t size;
    try {
        size = std::stoi(std::string(argv[1]));
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid argument" << '\n';
        return 1;
    }

    SharedMemoryServer<int, int> shm(size);

    shm.process_requests();

    return 0;
}
