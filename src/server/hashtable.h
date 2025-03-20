#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <optional>
#include <vector>

template <typename K, typename V>
class HashTable {
public:
    HashTable(size_t size)
        : size { size }
        , table(size)
    {
    }

    bool insert(K key, V value)
    {
        std::list<std::pair<K, V>> list = get_bucket(key);

        if (bucket_contains_key(list, key)) {
            return false;
        }

        list.insert(value);
        return true;
    }

    std::optional<V> get(K key)
    {
        std::list<std::pair<K, V>> list = get_bucket(key);

        auto iter = bucket_find_key(list, key);
        if (iter != list.end()) {
            return std::optional<V>((*iter).second);
        }

        return std::optional<V>();
    }

    bool remove(K key)
    {
        std::list<std::pair<K, V>> list = get_bucket(key);

        auto iter = bucket_find_key(list, key);
        if (iter != list.end()) {
            list.erase(iter);
            return true;
        }

        return false;
    }

    void print()
    {
        size_t index { 0 };
        for (auto bucket : table) {
            std::cout << "Bucket " << index++ << ": [";
            for (auto pair : bucket) {
                std::cout << "(" << pair.first << ", " << pair.second << ")";
            }
            std::cout << "]" << "\n";
        }
    }

private:
    size_t size;

    std::vector<std::list<std::pair<K, V>>> table;

    std::hash<K> hash_function;

    auto bucket_find_key(std::list<std::pair<K, V>> list, K key)
    {
        return std::find_if(list.begin(), list.end(), [&key](const std::pair<K, V>& pair) {
            return pair.first == key;
        });
    }

    bool bucket_contains_key(std::list<std::pair<K, V>> list, K key)
    {
        return bucket_find_key(list, key) != list.end();
    }

    std::list<std::pair<K, V>> get_bucket(K key)
    {
        size_t index = hash_function(key) % size;
        return table.at(index);
    }
};
