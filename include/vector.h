#ifndef _RPC_VECTOR_H
#define _RPC_VECTOR_H

#include <iostream>
#include <memory>
#include <stdexcept>

template <std::default_initializable T, typename Allocator = std::allocator<T>>
class Vector {
   public:
    Vector() = default;

    Vector(size_t size, const T &default_value = {}) : size_(size), capacity_(size) {
        data_ = allocator_.allocate(size);
        std::uninitialized_fill_n(data_, size, default_value);
    }

    Vector(const Vector &other) { set_data(other.data_, other.size_); }

    Vector(Vector &&other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~Vector() { del_data(); }

    Vector &operator=(const Vector &other) {
        if (this != &other) {
            del_data();
            set_data(other.data_, other.size_);
        }

        return *this;
    }

    Vector &operator=(Vector &&other) noexcept {
        if (this != &other) {
            del_data();

            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }

        return *this;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        T *new_data = allocator_.allocate(new_capacity);
        std::uninitialized_move_n(data_, size_, new_data);
        del_data();
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void resize(size_t new_size, const T &default_value = {}) {
        reserve(new_size);

        if (new_size < size_) {
            std::destroy(data_ + new_size, data_ + size_);
        } else if (new_size > size_) {
            std::uninitialized_fill(data_ + size_, data_ + new_size, default_value);
        }

        size_ = new_size;
    }

    void push_back(const T &value) {
        if (size_ == capacity_) {
            reserve(std::max(MIN_CAPACITY, capacity_ * 2));
        }

        new (data_ + size_) T{value};
        size_++;
    }

    void push_back(T &&value) {
        if (size_ == capacity_) {
            reserve(std::max(MIN_CAPACITY, capacity_ * 2));
        }

        new (data_ + size_) T{std::move(value)};
        size_++;
    }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    void emplace_back(Args &&...args) {
        if (size_ == capacity_) {
            reserve(std::max(MIN_CAPACITY, capacity_ * 2));
        }

        new (data_ + size_) T{std::forward<Args>(args)...};
        size_++;
    }

    void pop_back() {
        if (size_ == 0) {
            throw std::runtime_error("pop_back on empty vector");
        }
        size_--;
        data_[size_].~T();
    }

    T &operator[](size_t idx) {
        if (idx >= size_) {
            throw std::out_of_range("idx too large");
        }
        return data_[idx];
    }

    const T &operator[](size_t idx) const {
        if (idx >= size_) {
            throw std::out_of_range("idx too large");
        }
        return data_[idx];
    }

    const T &at(size_t idx) const {
        if (idx >= size_) {
            throw std::out_of_range("idx too large");
        }
        return data_[idx];
    }

    const T &back() const {
        if (size_ == 0) {
            throw std::out_of_range("vector is empty");
        }
        return data_[size_ - 1];
    }

    void clear() {
        std::destroy(data_, data_ + size_);
        size_ = 0;
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

   protected:
    void del_data() {
        if (data_) {
            std::destroy(data_, data_ + size_);
            allocator_.deallocate(data_, capacity_);
        }
    }

    void set_data(T *data, size_t size, bool move = false) {
        if (data) {
            data_ = allocator_.allocate(size);
            if (move) {
                std::uninitialized_move_n(data, size, data_);
            } else {
                std::uninitialized_copy_n(data, size, data_);
            }
        } else {
            data_ = nullptr;
        }
        size_ = size;
        capacity_ = size;
    }

    static constexpr size_t MIN_CAPACITY = 4;

    T *data_{nullptr};
    size_t size_{0};
    size_t capacity_{0};
    Allocator allocator_;
};

#endif  // _RPC_VECTOR_H