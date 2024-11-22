#ifndef _RPC_STRING_H
#define _RPC_STRING_H

#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

class String {
   public:
    String(const char* str = "") {
        size_ = std::strlen(str);  // Determine the length of the input string
        capacity_ = size_;         // Allocate one extra byte for the null terminator
        data_ = new char[capacity_ + 1];  // Allocate memory for the string (including
                                          // null terminator)
        std::strcpy(data_, str);  // Copy the input string into the allocated memory
    }

    String(const String& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = new char[capacity_ + 1];
        std::strcpy(data_, other.data_);
    }

    String(String&& other) noexcept {
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = other.data_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~String() {
        if (data_) {
            delete[] data_;
        }
    }

    String& operator=(const String& other) {
        if (this == &other) {
            return *this;
        }

        if (data_) {
            delete[] data_;
        }

        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = new char[capacity_ + 1];
        std::strcpy(data_, other.data_);

        return *this;
    }

    String& operator=(String&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        if (data_) {
            delete[] data_;
        }

        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = other.data_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;

        return *this;
    }

    void clear() {
        size_ = 0;
        data_[0] = '\0';
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        capacity_ = new_capacity;
        char* new_data = new char[capacity_ + 1];
        std::strcpy(new_data, data_);
        delete[] data_;
        data_ = new_data;
    }

    void resize(size_t new_size, char c = '\0') {
        if (new_size < size_) {
            data_[new_size] = '\0';
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            std::uninitialized_fill(data_ + size_, data_ + new_size, c);
        }
        size_ = new_size;
        data_[size_] = '\0';
    }

    void push_back(char c) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        data_[size_++] = c;
        data_[size_] = '\0';
    }

    void pop_back() {
        if (size_ > 0) {
            data_[--size_] = '\0';
        } else {
            throw std::out_of_range("pop_back() called on an empty string");
        }
    }

    char at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("String index out of range");
        }
        return data_[index];
    }

    void append(const char* str) {
        size_t new_size = size_ + std::strlen(str);

        reserve(new_size);

        std::strcat(data_, str);
        size_ = new_size;
    }

    bool empty() const { return size_ == 0; }

    const char* c_str() const { return data_; }

    size_t size() const { return size_; }

    size_t capacity() const { return capacity_; }

    String& operator+=(const String& other) {
        append(other.data_);
        return *this;
    }

    String& operator+=(char ch) {
        push_back(ch);
        return *this;
    }

    String operator+(const String& other) const {
        String result = *this;
        result.append(other.data_);
        return result;
    }

    String operator+(char ch) const {
        String result = *this;
        result.push_back(ch);
        return result;
    }

    char& operator[](size_t index) {
        if (index >= size_) {
            throw std::out_of_range("String index out of range");
        }
        return data_[index];
    }

    char operator[](size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("String index out of range");
        }
        return data_[index];
    }

    bool operator==(const String& other) const {
        return std::strcmp(data_, other.data_) == 0;
    }

    bool operator!=(const String& other) const {
        return std::strcmp(data_, other.data_) != 0;
    }

    bool operator<(const String& other) const {
        return std::strcmp(data_, other.data_) < 0;
    }
    bool operator>(const String& other) const {
        return std::strcmp(data_, other.data_) > 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const String& str) {
        os << str.data_;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, String& str) {
        str.clear();
        char ch;
        ch = is.get();
        while (ch != ' ' && ch != '\n') {
            str += ch;
            ch = is.get();
        }
        return is;
    }

   private:
    char* data_ = nullptr;
    size_t size_;
    size_t capacity_;
};

#endif  // _RPC_STRING_H