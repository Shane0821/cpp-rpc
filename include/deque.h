#ifndef _DEQUE_H
#define _DEQUE_H

#include <cstddef>
#include <iostream>
#include <memory>

template <typename T, size_t CHUNK_SIZE = 128>
class Deque {
   public:
    struct DequeIterator {
        T* cur_ = nullptr;    // pointer to the chunk
        T* first_ = nullptr;  // the begin of the chunk
        T* last_ = nullptr;   // the end of the chunk

        T** map_node_ = nullptr;  // pointer to the map node

        void set_node(T** new_node) {
            map_node_ = new_node;
            first_ = *new_node;
            last_ = first_ + CHUNK_SIZE;
        }

        T& operator*() const { return *cur_; }

        // Overload prefix ++ operator
        DequeIterator& operator++() {
            ++cur_;
            if (cur_ == last_) {
                set_node(map_node_ + 1);
                cur_ = first_;
            }
            return *this;
        }

        // Overload postfix ++ operator
        DequeIterator operator++(int) {
            DequeIterator tmp = *this;
            ++*this;
            return tmp;
        }

        // Overload prefix ++ operator
        DequeIterator& operator--() {
            if (cur_ == first_) {
                set_node(map_node_ - 1);
                cur_ = last_;
            }
            --cur_;
            return *this;
        }

        // Overload postfix -- operator
        DequeIterator operator--(int) {
            DequeIterator tmp = *this;
            --*this;
            return tmp;
        }

        DequeIterator& operator+=(size_t n) {
            size_t offset = n + (cur_ - first_);
            if (offset >= 0 && offset < CHUNK_SIZE) {
                cur_ += n;
            } else {
                size_t node_offset =
                    offset > 0 ? offset / CHUNK_SIZE : -((-offset - 1) / CHUNK_SIZE) - 1;
                set_node(map_node_ + node_offset);
                cur_ = first_ + (offset - node_offset * CHUNK_SIZE);
            }
            return *this;
        }

        DequeIterator& operator-=(size_t n) { return *this += -n; }

        DequeIterator operator+(size_t n) const {
            DequeIterator tmp = *this;
            return tmp += n;
        }

        DequeIterator operator-(size_t n) const {
            DequeIterator tmp = *this;
            return tmp -= n;
        }

        // random access (iterator can skip n steps)
        // invoke operator + ,operator *
        T& operator[](size_t n) const { return *(*this + n); }

        bool operator==(const DequeIterator& rhs) const { return cur_ == rhs.cur_; }
    };

    Deque(size_t n = 0, const T& val = {})
        requires std::default_initializable<T>
    {
        size_t num_chunks = n / CHUNK_SIZE + 1;
        map_size_ = num_chunks + 2;
        map_ = mapAllocator_.allocate(map_size_);

        auto map_start = map_ + (map_size_ - num_chunks) / 2;
        auto map_finish = map_start + num_chunks - 1;
        for (auto p = map_start; p <= map_finish; ++p) {
            *p = chunkAllocator_.allocate(CHUNK_SIZE);
        }
        start_.set_node(map_start);
        start_.cur_ = start_.first_;
        finish_.set_node(map_finish);
        finish_.cur_ = finish_.first_ + n % CHUNK_SIZE;

        for (auto i = start_.map_node_; i < finish_.map_node_; ++i) {
            std::uninitialized_fill_n(*i, CHUNK_SIZE, val);
        }
        // the end chunk may have space nodes, which don't need to have initialize value
        std::uninitialized_fill(finish_.first_, finish_.cur_, val);
    }

    Deque(const Deque& other) {
        map_size_ = other.map_size_;
        map_ = mapAllocator_.allocate(map_size_);
        auto new_start = map_ + (other.start_.map_node_ - other.map_);
        auto new_finish = map_ + (other.finish_.map_node_ - other.map_);

        for (auto p = new_start; p <= new_finish; ++p) {
            *p = chunkAllocator_.allocate(CHUNK_SIZE);
        }

        start_.set_node(new_start);
        start_.cur_ = start_.first_ + (other.start_.cur_ - other.start_.first_);
        finish_.set_node(new_finish);
        finish_.cur_ = finish_.first_ + (other.finish_.cur_ - other.finish_.first_);

        // copy the elements between start and finish
        for (auto p = new_start + 1; p < new_finish; ++p) {
            std::uninitialized_copy_n(*(other.map_ + (p - map_)), CHUNK_SIZE, *p);
        }
        // handle the start and finish chunk
        if (start_.map_node_ == finish_.map_node_) {
            std::uninitialized_copy(other.start_.cur_, other.finish_.cur_, start_.cur_);
        } else {
            std::uninitialized_copy(other.start_.cur_, other.start_.last_, start_.cur_);
            std::uninitialized_copy(other.finish_.first_, other.finish_.cur_,
                                    finish_.first_);
        }
    }

    Deque(Deque&& other) noexcept {
        start_ = other.start_;
        finish_ = other.finish_;
        map_ = other.map_;
        map_size_ = other.map_size_;

        other.map_ = nullptr;
        other.map_size_ = 0;
    }

    Deque& operator=(const Deque& other) {
        if (this == &other) {
            return *this;
        }

        this->~Deque();

        if (other.map_ == nullptr) {
            return *this;
        }

        map_size_ = other.map_size_;
        map_ = mapAllocator_.allocate(map_size_);
        auto new_start = map_ + (other.start_.map_node_ - other.map_);
        auto new_finish = map_ + (other.finish_.map_node_ - other.map_);

        for (auto p = new_start; p <= new_finish; ++p) {
            *p = chunkAllocator_.allocate(CHUNK_SIZE);
        }

        start_.set_node(new_start);
        start_.cur_ = start_.first_ + (other.start_.cur_ - other.start_.first_);
        finish_.set_node(new_finish);
        finish_.cur_ = finish_.first_ + (other.finish_.cur_ - other.finish_.first_);

        // copy the elements between start and finish
        for (auto p = new_start + 1; p < new_finish; ++p) {
            std::uninitialized_copy_n(*(other.map_ + (p - map_)), CHUNK_SIZE, *p);
        }
        // handle the start and finish chunk
        if (start_.map_node_ == finish_.map_node_) {
            std::uninitialized_copy(other.start_.cur_, other.finish_.cur_, start_.cur_);
        } else {
            std::uninitialized_copy(other.start_.cur_, other.start_.last_, start_.cur_);
            std::uninitialized_copy(other.finish_.first_, other.finish_.cur_,
                                    finish_.first_);
        }

        return *this;
    }

    Deque& operator=(Deque&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        this->~Deque();

        start_ = other.start_;
        finish_ = other.finish_;
        map_ = other.map_;
        map_size_ = other.map_size_;

        other.map_ = nullptr;
        other.map_size_ = 0;

        return *this;
    }

    ~Deque() {
        if (map_ == nullptr) {
            return;
        }

        if (start_.map_node_ == finish_.map_node_) {
            std::destroy(start_.cur_, finish_.cur_);
            chunkAllocator_.deallocate(start_.first_, CHUNK_SIZE);
        } else {
            for (auto p = start_.map_node_ + 1; p < finish_.map_node_; ++p) {
                std::destroy_n(*p, CHUNK_SIZE);
                chunkAllocator_.deallocate(*p, CHUNK_SIZE);
            }
            std::destroy(start_.cur_, start_.last_);
            chunkAllocator_.deallocate(start_.first_, CHUNK_SIZE);
            std::destroy(finish_.first_, finish_.cur_);
            chunkAllocator_.deallocate(finish_.first_, CHUNK_SIZE);
        }
        mapAllocator_.deallocate(map_, map_size_);
        map_ = nullptr;
    }

    void push_back(const T& val) {
        std::construct_at(finish_.cur_, val);
        if (finish_.cur_ + 1 == finish_.last_) {
            add_back_capacity();
        }
        ++finish_;
    }

    void push_back(T&& val) {
        std::construct_at(finish_.cur_, std::move(val));
        if (finish_.cur_ + 1 == finish_.last_) {
            add_back_capacity();
        }
        ++finish_;
    }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    void emplace_back(Args&&... args) {
        std::construct_at(finish_.cur_, std::forward<Args>(args)...);
        if (finish_.cur_ + 1 == finish_.last_) {
            add_back_capacity();
        }
        ++finish_;
    }

    void push_front(const T& val) {
        if (start_.cur_ == start_.first_) {
            add_front_capacity();
        }
        --start_;
        std::construct_at(start_.cur_, val);
    }

    void push_front(T&& val) {
        if (start_.cur_ == start_.first_) {
            add_front_capacity();
        }
        --start_;
        std::construct_at(start_.cur_, std::move(val));
    }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    void emplace_front(Args&&... args) {
        if (start_.cur_ == start_.first_) {
            add_front_capacity();
        }
        --start_;
        std::construct_at(start_.cur_, std::forward<Args>(args)...);
    }

    void pop_back() {
        if (finish_.cur_ == finish_.first_) {
            chunkAllocator_.deallocate(finish_.first_, CHUNK_SIZE);
        }
        --finish_;
        std::destroy_at(finish_.cur_);
    }

    void pop_front() {
        std::destroy_at(start_.cur_);
        if (start_.cur_ + 1 == start_.last_) {
            chunkAllocator_.deallocate(start_.first_, CHUNK_SIZE);
        }
        ++start_;
    }

    DequeIterator begin() { return start_; }
    DequeIterator end() { return finish_; }

    T& front() { return *start_; }
    T& back() { return *(finish_ - 1); }

    T& operator[](size_t n) {
        if (size() <= n) {
            throw std::out_of_range("Deque subscript out of range");
        }
        return start_[n];
    }

    bool empty() const { return size() == 0; }
    // TODO: use a variable to store the size
    size_t size() const {
        if (!map_) return 0;
        return CHUNK_SIZE * (finish_.map_node_ - start_.map_node_) +
               (finish_.cur_ - finish_.first_) + (start_.first_ - start_.cur_);
    }

   private:
    using MapAllocator = std::allocator<T*>;
    using ChunkAllocator = std::allocator<T>;

    void add_front_capacity() {
        if (start_.map_node_ == map_) {
            reallocate_map(map_size_ * 2);
        }
        *(start_.map_node_ - 1) = chunkAllocator_.allocate(CHUNK_SIZE);
    }

    void add_back_capacity() {
        if (finish_.map_node_ + 1 == map_ + map_size_) {
            reallocate_map(map_size_ * 2);
        }
        *(finish_.map_node_ + 1) = chunkAllocator_.allocate(CHUNK_SIZE);
    }

    void reallocate_map(size_t new_map_size) {
        auto new_map = mapAllocator_.allocate(new_map_size);
        auto new_start =
            new_map + (new_map_size - (finish_.map_node_ - start_.map_node_)) / 2;
        auto new_finish = new_start + (finish_.map_node_ - start_.map_node_);

        std::uninitialized_copy(start_.map_node_, finish_.map_node_ + 1, new_start);
        mapAllocator_.deallocate(map_, map_size_);

        map_ = new_map;
        map_size_ = new_map_size;
        start_.set_node(new_start);
        finish_.set_node(new_finish);
    }

    MapAllocator mapAllocator_;
    ChunkAllocator chunkAllocator_;
    DequeIterator start_;
    DequeIterator finish_;
    T** map_ = nullptr;  // pointer to the map
    size_t map_size_;    // number of map nodes, some of them may be empty
};

#endif  // _DEQUE_H