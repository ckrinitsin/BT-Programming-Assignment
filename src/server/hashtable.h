#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <sstream>
#include <vector>

/**
 * @class HashTable
 * @brief Represents a generic hashtable with simple operations.
 */
template <typename K, typename V>
class HashTable {
public:
    /**
     * @brief Constructs a new Hashtable.
     *
     * @param size The number of buckets of the table.
     */
    HashTable(size_t size)
        : size { size }
        , table(size)
        , bucket_mutexes(size)
    {
    }

    /**
     * @brief Insert a kv-pair into the hashtable.
     *
     * @param key The key to determine the bucket.
     * @param value The value to insert.
     * @return bool Successful insert of the pair.
     */
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

    /**
     * @brief Gets the value which corresponds to the key.
     *
     * @param key The key to look for.
     * @return std::optional The value, if the key could be found.
     */
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

    /**
     * @brief Removes the kv-pair which corresponds to the key.
     *
     * @param key The key to look for.
     * @return bool The pair could be removed successfully.
     */
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

    /**
     * @brief Constructs a string representation of the hashtable.
     *
     * @return std::string The string of the hashtable.
     */
    std::string string()
    {
        std::ostringstream output;

        size_t index { 0 };
        for (auto bucket : table) {
            output << "Bucket " << index << ": [";
            std::shared_lock<std::shared_mutex> lock(bucket_mutexes.at(index));
            for (auto pair : bucket) {
                output << "(" << pair.first << ", " << pair.second << ")";
            }
            output << "]" << "\n";
            ++index;
        }

        return output.str();
    }

private:
    /**
     * @brief The number of buckets.
     */
    size_t size;

    /**
     * @brief The hashtable.
     */
    std::vector<std::list<std::pair<K, V>>> table;

    /**
     * @brief A mutex for every button.
     */
    std::vector<std::shared_mutex> bucket_mutexes;

    /**
     * @brief The hashfunction to use for the bucket determination.
     */
    std::hash<K> hash_function;

    /**
     * @brief Finds the kv-pair inside a bucket.
     *
     * @param list The bucket.
     * @param key The key to look for.
     * @return auto The iterator element, which points to the kv-pair or list.end().
     */
    auto bucket_find_key(std::list<std::pair<K, V>>& list, K key)
    {
        return std::find_if(list.begin(), list.end(), [&key](const std::pair<K, V>& pair) {
            return pair.first == key;
        });
    }

    /**
     * @brief Checks if the bucket contains the key.
     *
     * @param list The bucket.
     * @param key The key to look for.
     * @return bool The bucket contains the key.
     */
    bool bucket_contains_key(std::list<std::pair<K, V>>& list, K key)
    {
        return list.begin() != list.end() && bucket_find_key(list, key) != list.end();
    }

    /**
     * @brief Uses the hashfunction and the key to determine, which bucket to use.
     *
     * @param key The key.
     * @return size_t The index of the bucket.
     */
    size_t get_bucket_index(K key) { return hash_function(key) % size; }
};
