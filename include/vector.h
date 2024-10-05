#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T, typename Alloc = std::allocator<T>>
class Vector {
   public:
    // Default constructor
    Vector() noexcept : size_(0), capacity_(0), data_(nullptr) {}

    Vector(size_t size) noexcept : size_(size), capacity_(size), data_(nullptr) {
        data_ = alloc_.allocate(capacity_);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::construct(alloc_, data_ + i);
        }
    }

    Vector(size_t size, T value) noexcept : size_(size), capacity_(size), data_(nullptr) {
        data_ = alloc_.allocate(capacity_);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::construct(alloc_, data_ + i, value);
        }
    }

    // Destructor
    ~Vector() noexcept {
        clear();
        std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);
    }

    // Copy constructor (deep copy)
    Vector(const Vector& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), data_(nullptr) {
        data_ = std::allocator_traits<Alloc>::allocate(alloc_, capacity_);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::construct(alloc_, data_ + i, other.data_[i]);
        }
    }

    // Move constructor (steals resources)
    Vector(Vector&& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;
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
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::construct(alloc_, data_ + i, other.data_[i]);
        }

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
        std::allocator_traits<Alloc>::construct(alloc_, data_ + size_, value);
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
        std::allocator_traits<Alloc>::construct(alloc_, data_ + size_,
                                                std::forward<Args>(args)...);
        ++size_;
    }

    void pop_back() noexcept {
        if (size_ > 0) {
            std::allocator_traits<Alloc>::destroy(alloc_, data_ + size_ - 1);
            --size_;
        }
    }

    // Reserve space for at least new_capacity elements
    void reserve(size_t new_capacity) noexcept {
        if (new_capacity > capacity_) {
            T* new_data = alloc_.allocate(new_capacity);
            for (size_t i = 0; i < size_; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, new_data + i,
                                                        std::move(data_[i]));
                std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
            }
            std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }

    // Resize the vector to contain new_size elements
    void resize(size_t new_size) noexcept {
        if (new_size < size_) {
            for (size_t i = new_size; i < size_; ++i) {
                std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
            }
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            for (size_t i = size_; i < new_size; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, data_ + i);
            }
        }
        size_ = new_size;
    }

    void resize(size_t new_size, T value) noexcept {
        if (new_size < size_) {
            for (size_t i = new_size; i < size_; ++i) {
                std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
            }
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                reserve(new_size);
            }
            for (size_t i = size_; i < new_size; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, data_ + i, value);
            }
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
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::destroy(alloc_, data_ + i);
        }
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

// // Example usage
// int main() {
//     Vector<int> v;
//     v.push_back(10);
//     v.push_back(20);
//     v.push_back(30);

//     // Copy constructor
//     Vector<int> v_copy = v;

//     std::cout << "Original vector: ";
//     for (size_t i = 0; i < v.size(); ++i) {
//         std::cout << v[i] << " ";
//     }
//     std::cout << "\nCopied vector: ";
//     for (size_t i = 0; i < v_copy.size(); ++i) {
//         std::cout << v_copy[i] << " ";
//     }
//     std::cout << std::endl;

//     // Move constructor
//     Vector<int> v_moved = std::move(v);
//     std::cout << "Moved vector: ";
//     for (size_t i = 0; i < v_moved.size(); ++i) {
//         std::cout << v_moved[i] << " ";
//     }
//     std::cout << std::endl;

//     // After the move, the original vector should be empty
//     std::cout << "Original vector after move: " << v.size() << " elements" <<
//     std::endl;

//     return 0;
// }