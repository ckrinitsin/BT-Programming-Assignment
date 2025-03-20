#include <string>

#include "hashtable.h"

int main(int argc, char* argv[])
{
    HashTable<int, std::string> hash_table { 5 };

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

}
