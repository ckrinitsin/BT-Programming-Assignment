#include <cstdint>
#include <stdexcept>
#include <string>

#include "hashtable.h"

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
  
    HashTable<int, std::string> hash_table { size };

    std::cout << "Add various kv-pairs" << '\n';
    hash_table.insert(1, "1");
    hash_table.insert(2, "2");
    hash_table.insert(3, "3");
    hash_table.insert(4, "4");
    hash_table.insert(5, "5");
    hash_table.insert(6, "6");
    hash_table.insert(7, "7");

    hash_table.print();

    std::cout << '\n';

    std::cout << "Value for key 8: " << hash_table.get(8).value_or("Key not found!") << '\n';
    std::cout << "Value for key 4: " << hash_table.get(4).value_or("Key not found!") << '\n';

    std::cout << '\n';
    std::cout << "Remove pair with key 5" << '\n';
    hash_table.remove(5);
    hash_table.print();

    return 0;
}
