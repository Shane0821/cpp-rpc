#ifndef _HASH_MAP_H
#define _HASH_MAP_H

#include <atomic>
#include <cstring>
#include <memory>
#include <optional>
#include <stdexcept>

template <typename Key, typename Value, typename Hash = std::hash<Key>>
    requires std::default_initializable<Value>
class HashMap {
   public:
    explicit HashMap(size_t capacity = 16, double load_factor = 0.75, Hash hash = Hash())
        : capacity_(capacity), size_(0), load_factor_(load_factor), hash_function_(hash) {
        if (capacity_ == 0) {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
        table_ = new Entry[capacity_];
    }

    // Copy constructor
    HashMap(const HashMap& other)
        : capacity_(other.capacity_),
          size_(other.size_),
          load_factor_(other.load_factor_),
          hash_function_(other.hash_function_) {
        table_ = new Entry[other.capacity_];
        std::copy(other.table_, other.table_ + other.capacity_, table_);
    }

    // Move constructor
    HashMap(HashMap&& other) noexcept
        : capacity_(other.capacity_),
          size_(other.size_),
          load_factor_(other.load_factor_),
          hash_function_(std::move(other.hash_function_)) {
        table_ = other.table_;
        other.table_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~HashMap() { delete[] table_; }

    // Copy assignment operator
    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return *this;
        }

        delete[] table_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        load_factor_ = other.load_factor_;
        hash_function_ = other.hash_function_;
        table_ = new Entry[capacity_];
        std::copy(other.table_, other.table_ + other.capacity_, table_);
        return *this;
    }

    // Move assignment operator
    HashMap& operator=(HashMap&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] table_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        load_factor_ = other.load_factor_;
        hash_function_ = std::move(other.hash_function_);
        table_ = other.table_;
        other.table_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    // Insert a key-value pair into the hashmap
    void insert(const Key& key, const Value& value) {
        if (needs_rehash()) {
            rehash();
        }

        auto idx = hash_function_(key) % capacity_;
        while (table_[idx].occupied_) {
            if (table_[idx].key == key) {
                table_[idx].value = value;
                return;
            }
            idx = (idx + 1) % capacity_;
        }

        table_[idx] = Entry(key, value);
        ++size_;
    }

    void insert(Key&& key, Value&& value) {
        if (needs_rehash()) {
            rehash();
        }

        auto idx = hash_function_(key) % capacity_;
        while (table_[idx].occupied_) {
            if (table_[idx].key_ == key) {
                table_[idx].value_ = std::move(value);
                return;
            }
            idx = (idx + 1) % capacity_;
        }

        table_[idx] = Entry(std::move(key), std::move(value));
        ++size_;
    }

    // Remove a key-value pair by key
    void erase(const Key& key) {
        if (auto idx = find_index(key); idx.has_value()) {
            table_[idx.value()].occupied_ = false;
            --size_;
            return;
        }

        throw std::out_of_range("Key not found");
    }

    // Check if the hashmap contains a key
    bool contains(const Key& key) const { return find_index(key) != std::nullopt; }

    // Get the number of elements in the hashmap
    size_t size() const { return size_; }

    // Get the capacity of the hashmap
    size_t capacity() const { return capacity_; }

    // Check if the hashmap is empty
    bool empty() const { return size_ == 0; }

    [[nodiscard]] Value& operator[](const Key& key) {
        if (auto idx = find_index(key); idx.has_value()) {
            return table_[idx.value()].value_;
        }

        if (needs_rehash()) {
            rehash();
        }

        auto idx = hash_function_(key) % capacity_;
        while (table_[idx].occupied_) {
            idx = (idx + 1) % capacity_;
        }
        table_[idx] = Entry(key, Value{});
        ++size_;
        return table_[idx].value_;
    }

   private:
    struct Entry {
        Key key_;
        Value value_;
        bool occupied_;

        Entry() : occupied_(false) {}
        Entry(const Key& k, const Value& v) : key_(k), value_(v), occupied_(true) {}
        Entry(Key&& k, Value&& v)
            : key_(std::move(k)), value_(std::move(v)), occupied_(true) {}
    };

    // Check if rehashing is needed based on load factor
    bool needs_rehash() const { return size_ >= capacity_ * load_factor_; }

    // Rehash the table when load factor threshold is reached
    void rehash() {
        auto new_capacity = capacity_ ? capacity_ * 2 : static_cast<size_t>(16);
        auto new_table = new Entry[new_capacity];
        for (size_t i = 0; i < capacity_; ++i) {
            if (table_[i].occupied_) {
                auto& entry = table_[i];
                auto index = hash_function_(entry.key_) % new_capacity;
                while (new_table[index].occupied_) {
                    index = (index + 1) % new_capacity;
                }
                new_table[index] = entry;
            }
        }
        delete[] table_;
        table_ = new_table;
        capacity_ = new_capacity;
    }

    // Find the index of a key in the table
    std::optional<size_t> find_index(const Key& key) const {
        if (table_ == nullptr) {
            return std::nullopt;  // Table is empty
        }

        size_t idx = hash_function_(key) % capacity_;
        size_t start_idx = idx;

        while (table_[idx].occupied_) {
            if (table_[idx].key_ == key) {
                return idx;
            }
            idx = (idx + 1) % capacity_;
            if (idx == start_idx) {
                break;
            }
        }

        return std::nullopt;  // Not found
    }

    Entry* table_ = nullptr;
    size_t size_;
    size_t capacity_;
    double load_factor_;
    Hash hash_function_;
};

#endif