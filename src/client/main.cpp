#include "shared_memory.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <utility>

void sendRequest(
    SharedMemory* shared_memory,
    Operations type,
    std::pair<const std::string&, const std::string&> arguments)
{
    shared_memory->request.type = type;
    strncpy(shared_memory->request.key, serialize(arguments.first).c_str(), MAX_KEY_SIZE);
    strncpy(shared_memory->request.value, serialize(arguments.second).c_str(), MAX_VALUE_SIZE);
    shared_memory->processed = false;

    std::cout << "Command sent" << '\n';

    sleep(2);

    if (type == Operations::GET) {
        std::string result(shared_memory->response);
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
    sendRequest(shared_memory, INSERT, std::pair(serialize(10), serialize(5)));

    std::cout << "Start printing.." << '\n';
    sendRequest(shared_memory, PRINT, std::pair(serialize(10), serialize(5)));

    munmap(shared_memory, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
