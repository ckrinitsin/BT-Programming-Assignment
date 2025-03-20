#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>

#include "hashtable.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::printf("One argument required");
        return 1;
    }

    uint32_t size;
    try {
        size = std::stoi(std::string(argv[1]));
    } catch (const std::invalid_argument& e) {
        std::printf("Invalid argument");
        return 1;
    }


    return 0;
}
