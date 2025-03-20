#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <vector>

template <typename K, typename V>
class HashTable {
public:
    HashTable(size_t size)
        : size { size }
        , table(size)
        , bucket_mutexes(size)
    {
    }

    bool insert(K key, V value)
    {
        size_t index = get_bucket_index(key);
        std::unique_lock<std::shared_mutex> lock(bucket_mutexes.at(index));

        std::list<std::pair<K, V>>& list = table.at(index);

        if (bucket_contains_key(list, key)) {
            return false;
        }

        list.push_back(std::pair(key, value));

        return true;
    }

    std::optional<V> get(K key)
    {
        size_t index = get_bucket_index(key);
        std::shared_lock<std::shared_mutex> lock(bucket_mutexes.at(index));

        std::list<std::pair<K, V>>& list = table.at(index);

        auto iter = bucket_find_key(list, key);
        if (iter != list.end()) {
            return std::optional<V>((*iter).second);
        }

        return std::optional<V>();
    }

    bool remove(K key)
    {
        size_t index = get_bucket_index(key);
        std::unique_lock<std::shared_mutex> lock(bucket_mutexes.at(index));

        std::list<std::pair<K, V>>& list = table.at(index);

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
            std::cout << "Bucket " << index << ": [";
            std::shared_lock<std::shared_mutex> lock(bucket_mutexes.at(index));
            for (auto pair : bucket) {
                std::cout << "(" << pair.first << ", " << pair.second << ")";
            }
            std::cout << "]" << "\n";
            ++index;
        }
    }

private:
    size_t size;

    std::vector<std::list<std::pair<K, V>>> table;

    std::vector<std::shared_mutex> bucket_mutexes;

    std::hash<K> hash_function;

    auto bucket_find_key(std::list<std::pair<K, V>>& list, K key)
    {
        return std::find_if(list.begin(), list.end(), [&key](const std::pair<K, V>& pair) {
            return pair.first == key;
        });
    }

    bool bucket_contains_key(std::list<std::pair<K, V>>& list, K key)
    {
        return list.begin() != list.end() && bucket_find_key(list, key) != list.end();
    }

    size_t get_bucket_index(K key) { return hash_function(key) % size; }
};
