#ifndef _RPC_STRING_H
#define _RPC_STRING_H

#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

class String {
   public:
    String() {
        sso_data_[0] = '\0';
        is_sso_ = true;
    }

    String(const char* str) {
        size_t len = std::strlen(str);  // Determine the length of the input string
        if (len <= SSO_THRESHOLD) {
            set_sso(str, len);
        } else {
            set_heap(str, len);
        }
    }

    String(const String& other) {
        if (other.is_sso_) {
            set_sso(other.sso_data_, std::strlen(other.sso_data_));
        } else {
            set_heap(other.heap_data_, other.heap_size_);
        }
    }

    String(String&& other) noexcept : is_sso_(other.is_sso_) {
        if (other.is_sso_) {
            std::strcpy(sso_data_, other.sso_data_);
            other.sso_data_[0] = '\0';
        } else {
            heap_size_ = other.heap_size_;
            heap_capacity_ = other.heap_capacity_;
            heap_data_ = other.heap_data_;

            other.heap_data_ = nullptr;
            other.heap_size_ = 0;
            other.heap_capacity_ = 0;
        }
    }

    ~String() {
        if (!is_sso_ && heap_data_) {
            delete[] heap_data_;
        }
    }

    String& operator=(const String& other) {
        if (this == &other) {
            return *this;
        }

        is_sso_ = other.is_sso_;

        if (is_sso_) {
            set_sso(other.sso_data_, std::strlen(other.sso_data_));
        } else {
            if (heap_data_) {
                delete[] heap_data_;
            }

            set_heap(other.heap_data_, other.heap_size_);
        }

        return *this;
    }

    String& operator=(String&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        is_sso_ = other.is_sso_;

        if (is_sso_) {
            std::strcpy(sso_data_, other.sso_data_);
            other.sso_data_[0] = '\0';
            return *this;
        }

        if (heap_data_) {
            delete[] heap_data_;
        }

        heap_size_ = other.heap_size_;
        heap_capacity_ = other.heap_capacity_;
        heap_data_ = other.heap_data_;

        other.heap_data_ = nullptr;
        other.heap_size_ = 0;
        other.heap_capacity_ = 0;

        return *this;
    }

    void clear() {
        if (!is_sso_) {
            heap_size_ = 0;
            heap_data_[0] = '\0';
        } else {
            sso_data_[0] = '\0';
        }
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity()) {
            return;
        }

        if (is_sso_) {
            set_heap(sso_data_, std::strlen(sso_data_));
        }

        heap_capacity_ = new_capacity;
        char* new_data = new char[heap_capacity_ + 1];
        std::strcpy(new_data, heap_data_);
        delete[] heap_data_;
        heap_data_ = new_data;
    }

    void resize(size_t new_size, char c = '\0') {
        if (is_sso_) {
            if (new_size <= SSO_THRESHOLD) {
                sso_data_[new_size] = '\0';
                return;
            }

            set_heap(sso_data_, std::strlen(sso_data_));
        }

        if (new_size > heap_size_) {
            if (new_size > heap_capacity_) {
                reserve(new_size);
            }
            std::uninitialized_fill(heap_data_ + heap_size_, heap_data_ + new_size, c);
        }
        heap_size_ = new_size;
        heap_data_[heap_size_] = '\0';
    }

    void push_back(char c) {
        if (size() >= capacity()) {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }

        if (is_sso_) {
            auto len = size();
            sso_data_[len] = c;
            sso_data_[len + 1] = '\0';
        } else {
            heap_data_[heap_size_++] = c;
            heap_data_[heap_size_] = '\0';
        }
    }

    void pop_back() {
        if (size() > 0) {
            if (is_sso_) {
                sso_data_[size() - 1] = '\0';
            } else {
                heap_data_[--heap_size_] = '\0';
            }
        } else {
            throw std::out_of_range("pop_back() called on an empty string");
        }
    }

    char at(size_t index) const {
        if (index >= size()) {
            throw std::out_of_range("String index out of range");
        }
        return is_sso_ ? sso_data_[index] : heap_data_[index];
    }

    void append(const char* str) {
        size_t new_size = size() + std::strlen(str);

        reserve(new_size);

        if (is_sso_) {
            std::strcat(sso_data_, str);
        } else {
            std::strcat(heap_data_, str);
            heap_size_ = new_size;
        }
    }

    bool empty() const { return size() == 0; }

    const char* c_str() const { return is_sso_ ? sso_data_ : heap_data_; }

    size_t size() const { return is_sso_ ? std::strlen(sso_data_) : heap_size_; }

    size_t capacity() const { return is_sso_ ? SSO_THRESHOLD : heap_capacity_; }

    String& operator+=(const String& other) {
        append(other.is_sso_ ? other.sso_data_ : other.heap_data_);
        return *this;
    }

    String& operator+=(char ch) {
        push_back(ch);
        return *this;
    }

    String operator+(const String& other) const {
        String result = *this;
        result.append(other.is_sso_ ? other.sso_data_ : other.heap_data_);
        return result;
    }

    String operator+(char ch) const {
        String result = *this;
        result.push_back(ch);
        return result;
    }

    char& operator[](size_t index) {
        if (index >= size()) {
            throw std::out_of_range("String index out of range");
        }
        return is_sso_ ? sso_data_[index] : heap_data_[index];
    }

    char operator[](size_t index) const {
        if (index >= size()) {
            throw std::out_of_range("String index out of range");
        }
        return is_sso_ ? sso_data_[index] : heap_data_[index];
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

    friend std::ostream& operator<<(std::ostream& os, const String& str) {
        os << str.c_str();
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
    void set_sso(const char* str, size_t len) {
        std::strcpy(sso_data_, str);  // Copy the string into SSO storage
        sso_data_[len] = '\0';        // Null-terminate
        is_sso_ = true;               // Mark as SSO
    }

    void set_heap(const char* str, size_t len) {
        auto tmp = new char[len + 1];
        std::strcpy(tmp, str);  // Copy the string into heap storage
        tmp[len] = '\0';        // Null-terminate
        heap_data_ = tmp;
        heap_size_ = len;
        heap_capacity_ = len;
        is_sso_ = false;  // Mark as heap
    }

    static constexpr size_t SSO_THRESHOLD = 15;  // Threshold for SSO

    union {
        char sso_data_[SSO_THRESHOLD + 1];  // +1 for null terminator
        struct {
            char* heap_data_ = nullptr;
            size_t heap_size_;
            size_t heap_capacity_;
        };
    };

    bool is_sso_ = false;
};

#endif  // _RPC_STRING_H