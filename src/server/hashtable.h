#pragma once

#include <list>
#include <vector>

template <typename K, typename V>
class HashTable {
public:
    HashTable(size_t size)
        : size { size }
        , table(size)
    {
    }

private:
    size_t size;

    std::vector<std::list<std::pair<K, V>>> table;
};
