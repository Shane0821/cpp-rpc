#ifndef _CORO_GENERATOR_H
#define _CORO_GENERATOR_H

#include <coroutine>
#include <exception>
#include <iterator>
#include <type_traits>
#include <ranges>

namespace coro {
template <typename T>
class generator;

template <typename T>
class generator_promise {
   public:
    using value_type = std::remove_reference_t<T>;
    using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
    using pointer_type = value_type*;

    generator_promise() = default;

    auto get_return_object() noexcept {
        return generator<T>{
            std::coroutine_handle<generator_promise<T>>::from_promise(*this)};
    }

    auto initial_suspend() const { return std::suspend_always{}; }

    auto final_suspend() const noexcept(true) { return std::suspend_always{}; }

    // SFIANE
    template <typename U = T,
              std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0>
    auto yield_value(value_type& value) noexcept {
        m_value = std::addressof(value);
        return std::suspend_always{};
    }

    auto yield_value(value_type&& value) noexcept {
        m_value = std::addressof(value);
        return std::suspend_always{};
    }

    auto unhandled_exception() { m_exception = std::current_exception(); }

    auto return_void() noexcept {}

    decltype(auto) value() const noexcept {
        return static_cast<reference_type>(*m_value);
    }

    template <typename U>
    auto await_transform(U&& value) -> std::suspend_never = delete;

    auto rethrow_if_exception() -> void {
        if (m_exception) {
            std::rethrow_exception(m_exception);
        }
    }

   private:
    pointer_type m_value{nullptr};
    std::exception_ptr m_exception{nullptr};
};

struct generator_sentinel {};

template <typename T>
class generator_iterator {
   public:
    using coroutine_handle = std::coroutine_handle<generator_promise<T>>;
    using value_type = typename generator_promise<T>::value_type;
    using reference = typename generator_promise<T>::reference_type;
    using pointer = typename generator_promise<T>::pointer_type;

    generator_iterator() noexcept {}

    explicit generator_iterator(coroutine_handle coroutine) noexcept
        : m_coroutine(coroutine) {}

    friend auto operator==(const generator_iterator& it, generator_sentinel) noexcept {
        return it.m_coroutine == nullptr || it.m_coroutine.done();
    }

    friend auto operator!=(const generator_iterator& it,
                           generator_sentinel s) noexcept -> bool {
        return !(it == s);
    }

    friend auto operator==(generator_sentinel s,
                           const generator_iterator& it) noexcept -> bool {
        return (it == s);
    }

    friend auto operator!=(generator_sentinel s,
                           const generator_iterator& it) noexcept -> bool {
        return it != s;
    }

    generator_iterator& operator++() {
        m_coroutine.resume();
        if (m_coroutine.done()) {
            m_coroutine.promise().rethrow_if_exception();
        }

        return *this;
    }

    auto operator++(int) { (void)operator++(); }

    reference operator*() const noexcept { return m_coroutine.promise().value(); }

    pointer operator->() const noexcept { return std::addressof(operator*()); }

   private:
    coroutine_handle m_coroutine{nullptr};
};

template <typename T>
class generator : public std::ranges::view_base {
    friend class generator_promise<T>;

   public:
    using promise_type = generator_promise<T>;
    using iterator = generator_iterator<T>;
    using sentinel = generator_sentinel;

    generator() noexcept : m_coroutine(nullptr) {}

    generator(const generator&) = delete;
    generator(generator&& other) noexcept : m_coroutine(other.m_coroutine) {
        other.m_coroutine = nullptr;
    }

    auto operator=(const generator&) = delete;
    decltype(auto) operator=(generator&& other) noexcept {
        m_coroutine = other.m_coroutine;
        other.m_coroutine = nullptr;

        return *this;
    }

    ~generator() {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    auto begin() {
        if (m_coroutine != nullptr) {
            m_coroutine.resume();
            if (m_coroutine.done()) {
                m_coroutine.promise().rethrow_if_exception();
            }
        }

        return iterator{m_coroutine};
    }

    auto end() noexcept { return sentinel{}; }

   private:
    explicit generator(std::coroutine_handle<promise_type> coroutine) noexcept
        : m_coroutine(coroutine) {}

    std::coroutine_handle<promise_type> m_coroutine;
};
}  // namespace coro

#endif  // _CORO_GENERATOR_H