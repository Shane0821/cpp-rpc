#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T, typename Alloc = std::allocator<T>>
class Vector {
   public:
    // Default constructor
    Vector() noexcept : size_(0), capacity_(0), data_(nullptr) {}

    Vector(size_t size) noexcept : size_(size), capacity_(size), data_(nullptr) {
        static_assert(std::is_default_constructible<T>::value,
                      "Type must be default constructible");

        data_ = alloc_.allocate(capacity_);
        std::__uninitialized_default_n(data_, size_);
    }

    Vector(size_t size, const T& value) noexcept
        : size_(size), capacity_(size), data_(nullptr) {
        data_ = alloc_.allocate(capacity_);
        std::uninitialized_fill_n(data_, size_, value);
    }

    // Copy constructor (deep copy)
    Vector(const Vector& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), data_(nullptr) {
        data_ = std::allocator_traits<Alloc>::allocate(alloc_, capacity_);
        std::uninitialized_copy(other.data_, other.data_ + size_, data_);
    }

    // Move constructor (steals resources)
    Vector(Vector&& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;
    }

    // Destructor
    ~Vector() noexcept {
        clear();
        std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);
    }

    // Copy assignment operator (deep copy)
    Vector& operator=(const Vector& other) noexcept {
        if (this == &other) {
            return *this;  // No self-assignment
        }

        // Free existing resources
        clear();
        std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);

        // Allocate new memory and copy elements
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = std::allocator_traits<Alloc>::allocate(alloc_, capacity_);
        std::uninitialized_copy(other.data_, other.data_ + size_, data_);

        return *this;
    }

    // Move assignment operator (steals resources)
    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) {
            return *this;  // No self-assignment
        }

        // Free existing resources
        clear();
        std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);

        // Steal the resources from the other vector
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = other.data_;

        // Nullify the moved-from vector
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;

        return *this;
    }

    // Add an element to the end of the vector
    void push_back(const T& value) noexcept {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::construct_at(data_ + size_, value);
        ++size_;
    }

    // Construct an element in-place at the end of the vector
    template <typename... Args>
    void emplace_back(Args&&... args) noexcept {
        static_assert(std::is_constructible<T, Args...>::value,
                      "Cannot construct element in-place with these arguments");

        if (size_ == capacity_) {
            // Increase capacity if full
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        // Forward the arguments to construct the element in-place
        std::construct_at(data_ + size_, std::forward<Args>(args)...);
        ++size_;
    }

    void pop_back() noexcept {
        if (size_ > 0) {
            std::destroy_at(data_ + size_ - 1);
            --size_;
        }
    }

    // Reserve space for at least new_capacity elements
    void reserve(size_t new_capacity) noexcept {
        if (new_capacity > capacity_) {
            T* new_data = alloc_.allocate(new_capacity);
            std::uninitialized_move(data_, data_ + size_, new_data);
            std::destroy(data_, data_ + size_);
            std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }

    // Resize the vector to contain new_size elements
    void resize(size_t new_size) noexcept {
        if (new_size < size_) {
            std::destroy(data_ + new_size, data_ + size_);
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            std::__uninitialized_default(data_ + size_, data_ + new_size);
        }
        size_ = new_size;
    }

    void resize(size_t new_size, const T& value) noexcept {
        if (new_size < size_) {
            std::destroy(data_ + new_size, data_ + size_);
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            std::uninitialized_fill(data_ + size_, data_ + new_size, value);
        }
        size_ = new_size;
    }

    T& back() {
        if (size_ == 0) {
            throw std::out_of_range("Vector is empty");
        }
        return data_[size_ - 1];
    }

    // Access elements with bounds checking
    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // Access elements without bounds checking
    T& operator[](size_t index) noexcept { return data_[index]; }

    // Clear the vector (destroy all elements)
    void clear() noexcept {
        std::destroy(data_, data_ + size_);
        size_ = 0;
    }

    [[nodiscard]] size_t size() const noexcept { return size_; }
    [[nodiscard]] size_t capacity() const noexcept { return capacity_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

   private:
    size_t size_;
    size_t capacity_;
    T* data_;
    Alloc alloc_;
};