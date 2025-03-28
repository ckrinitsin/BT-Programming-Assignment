#include "client.h"
#include "shared_memory.h"
#include <optional>

Client::Client()
{
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cout << "Server not running" << '\n';
        exit(-1);
    }

    shared_memory =
        (SharedMemory*)mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
}

Client::~Client()
{
    munmap(shared_memory, sizeof(SharedMemory));
    close(shm_fd);
}

void Client::start_client()
{
    while (true) {
        char operation;
        int k, v;
        int index = -1;
        std::cout << "Choose the operation (i: Insert, g: Get, r: Remove, p: Print, e: Exit)"
                  << '\n';
        std::cin >> operation;

        switch (operation) {
        case 'e':
            return;

        case 'i':
            std::cout << "Insert: Enter the k-v pair(<int> <int>):" << '\n';
            if (!(std::cin >> k >> v)) {
                std::cout << "Invalid input" << '\n';
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            };
            index = send_request(
                shared_memory, INSERT, std::optional(serialize(k)), std::optional(serialize(v)));
            break;

        case 'g': {
            std::cout << "Get: Enter the key(<int>):" << '\n';
            if (!(std::cin >> k)) {
                std::cout << "Invalid input" << '\n';
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            };
            index = send_request(shared_memory, GET, std::optional(serialize(k)), std::nullopt);
            break;
        }

        case 'r':
            std::cout << "Remove: Enter key(<int>):" << '\n';
            if (!(std::cin >> k)) {
                std::cout << "Invalid input" << '\n';
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            };
            index = send_request(shared_memory, DELETE, std::optional(serialize(k)), std::nullopt);
            break;

        case 'p':
            index = send_request(shared_memory, PRINT, std::nullopt, std::nullopt);
            break;

        default:
            break;
        }

        if (index != -1) {
            std::string response = process_respond(shared_memory, index);
            std::cout << response << '\n';
        }
        std::cout << '\n';
    }
}

bool Client::request_processed(SharedMemory* shared_memory, int index)
{
    if (index >= QUEUE_SIZE || index < 0) {
        return false;
    }

    return shared_memory->request[index].status == PROCESSED;
}

int Client::send_request(
    SharedMemory* shared_memory,
    Operations type,
    std::optional<const std::string> k,
    std::optional<const std::string> v)
{
    int index;

    pthread_mutex_lock(&shared_memory->mutex);

    while (shared_memory->request[shared_memory->tail].status != FREE) {
        pthread_cond_wait(&shared_memory->cond_var, &shared_memory->mutex);
    }

    index = shared_memory->tail;
    Request* request = &shared_memory->request[index];
    request->type = type;
    request->status = SENT;
    strncpy(request->key, k.value_or("null").c_str(), MAX_KEY_SIZE);
    strncpy(request->value, v.value_or("null").c_str(), MAX_VALUE_SIZE);
    shared_memory->tail = (1 + shared_memory->tail) % QUEUE_SIZE;
    pthread_cond_signal(&shared_memory->cond_var);

    pthread_mutex_unlock(&shared_memory->mutex);

    return index;
}

std::string Client::process_respond(SharedMemory* shared_memory, int index)
{
    if (index >= QUEUE_SIZE || index < 0) {
        return std::string();
    }

    pthread_mutex_lock(&shared_memory->mutex);

    while (!request_processed(shared_memory, index)) {
        pthread_cond_wait(&shared_memory->cond_var, &shared_memory->mutex);
    }
    std::string result(shared_memory->request[index].response);
    shared_memory->request[index].status = FREE;
    pthread_mutex_unlock(&shared_memory->mutex);
    return result;
}
