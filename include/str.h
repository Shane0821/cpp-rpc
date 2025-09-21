#ifndef _RPC_STRING_H
#define _RPC_STRING_H

#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

class String {
   public:
    String(const char *ptr = "") {
        int len = std::strlen(ptr);
        if (len <= MIN_CAPACITY) {
            set_stack_data(ptr, len);
        } else {
            set_heap_data(ptr, len);
        }
    }

    String(const String &other) {
        if (other.is_heap_) {
            set_heap_data(other.heap_data_, other.size_);
        } else {
            set_stack_data(other.stack_data_, other.size_);
        }
    }

    String(String &&other) noexcept {
        size_ = other.size_;
        is_heap_ = other.is_heap_;
        if (is_heap_) {
            heap_data_ = other.heap_data_;
            heap_capacity_ = other.heap_capacity_;
            other.set_heap_data("", 0);
        } else {
            set_stack_data(other.stack_data_, other.size_);
            other.set_stack_data("", 0);
        }
    }

    ~String() { del_data(); }

    String &operator=(const String &other) {
        if (this != &other) {
            del_data();
            size_ = other.size_;
            is_heap_ = other.is_heap_;
            if (is_heap_) {
                set_heap_data(other.heap_data_, other.size_);
            } else {
                set_stack_data(other.stack_data_, other.size_);
            }
        }
        return *this;
    }

    String &operator=(String &&other) noexcept {
        if (this != &other) {
            del_data();
            size_ = other.size_;
            is_heap_ = other.is_heap_;
            if (is_heap_) {
                heap_data_ = other.heap_data_;
                heap_capacity_ = other.heap_capacity_;
                other.set_heap_data("", 0);
            } else {
                set_stack_data(other.stack_data_, other.size_);
                other.set_stack_data("", 0);
            }
        }
        return *this;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity()) {
            return;
        }
        auto new_data = new char[new_capacity + 1]{0};
        std::strcpy(new_data, data());
        del_data();
        heap_data_ = new_data;
        heap_capacity_ = new_capacity;
        is_heap_ = true;
    }

    void resize(size_t new_size, char c = '\0') {
        reserve(new_size);
        if (new_size > size_) {
            memset(data() + size_, c, new_size - size_);
        }
        data()[new_size] = '\0';
        size_ = new_size;
    }

    void push_back(char c) {
        if (size_ == capacity()) {
            reserve(size_ * 2);
        }
        data()[size_] = c;
        size_++;
    }

    void pop_back() {
        if (size_ == 0) {
            throw std::runtime_error("pop back on empty vector");
        }
        size_--;
        data()[size_] = '\0';
    }

    void append(const char *ptr) {
        if (ptr == nullptr) return;
        size_t len = strlen(ptr);
        reserve(size_ + len);
        strcpy(data() + size_, ptr);
        size_ += len;
    }

    void clear() {
        data()[0] = '\0';
        size_ = 0;
    }

    char at(size_t index) const {
        if (index >= size()) {
            throw std::out_of_range("index out of range");
        }
        return c_str()[index];
    }

    char &at(size_t index) {
        if (index >= size()) {
            throw std::out_of_range("index out of range");
        }
        return data()[index];
    }

    char &operator[](size_t idx) {
        if (idx >= size_) {
            throw std::out_of_range("idx too large");
        }
        return data()[idx];
    }

    char operator[](size_t idx) const {
        if (idx >= size_) {
            throw std::out_of_range("idx too large");
        }
        return c_str()[idx];
    }

    String &operator+=(char c) {
        push_back(c);
        return *this;
    }

    String &operator+=(const String &other) {
        append(other.c_str());
        return *this;
    }

    String operator+(char c) const {
        String tmp = *this;
        tmp.push_back(c);
        return tmp;
    }
    
    String operator+(const String &other) const {
        String tmp = *this;
        tmp.append(other.c_str());
        return tmp;
    }

    bool operator==(const String& other) const {
        return std::strcmp(c_str(), other.c_str()) == 0;
    }
    bool operator!=(const String& other) const { return !(*this == other); }

    bool operator<(const String& other) const {
        return std::strcmp(c_str(), other.c_str()) < 0;
    }
    bool operator>(const String& other) const {
        return std::strcmp(c_str(), other.c_str()) > 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const String &s) {
        os << s.c_str();
        return os;
    }

    friend std::istream &operator>>(std::istream &is, String &str) {
        str.clear();
        char ch;
        ch = is.get();
        while (ch != ' ' && ch != '\n') {
            str += ch;
            ch = is.get();
        }
        return is;
    }

    char *data() { return is_heap_ ? heap_data_ : stack_data_; }
    const char *c_str() const { return is_heap_ ? heap_data_ : stack_data_; }
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    size_t capacity() const { return is_heap_ ? heap_capacity_ : MIN_CAPACITY; }

   protected:
    void set_heap_data(const char *ptr, size_t size) {
        heap_data_ = new char[size + 1]{0};
        std::strcpy(heap_data_, ptr);
        size_ = size;
        heap_capacity_ = size;
        is_heap_ = true;
    }

    void set_stack_data(const char *ptr, size_t size) {
        strcpy(stack_data_, ptr);
        size_ = size;
        is_heap_ = false;
    }

    void del_data() {
        if (is_heap_) delete[] heap_data_;
    }

    static constexpr size_t MIN_CAPACITY{15};

    union {
        struct {
            char *heap_data_;
            size_t heap_capacity_;
        };
        struct {
            char stack_data_[MIN_CAPACITY + 1]{0};
        };
    };
    size_t size_;
    bool is_heap_{false};
};

#endif  // _RPC_STRING_H