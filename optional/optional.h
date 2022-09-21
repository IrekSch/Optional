#include <stdexcept>
#include <utility>

class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;

    Optional(const T& value) {
        *this = value;
    }

    Optional(T&& value) {
        *this = std::move(value);
    }

    Optional(const Optional& other) {
        *this = other;
    }

    Optional(Optional&& other) noexcept {
        *this = std::move(other);
    }

    Optional& operator=(const T& value) {
        Forward(value);
        return *this;
    }

    Optional& operator=(T&& rhs) {
        Forward(std::move(rhs));
        return *this;
    }

    Optional& operator=(const Optional& rhs) {
        if (rhs.HasValue()) {
            Forward(rhs.Value());
        }
        else {
            Reset();
        }

        return *this;
    }

    Optional& operator=(Optional&& rhs) {
        if (rhs.HasValue()) {
            Forward(std::move(rhs.Value()));
        }
        else {
            Reset();
        }

        return *this;
    }

    ~Optional() {
        Reset();
    }

    bool HasValue() const {
        return is_initialized_;
    }

    T& operator*() {
        return *ptr_;
    }

    const T& operator*() const {
        return *ptr_;
    }

    T* operator->() {
        return ptr_;
    }

    const T* operator->() const {
        return ptr_;
    }

    T& Value() {
        if (!is_initialized_) {
            throw BadOptionalAccess{};
        }
        return *ptr_;
    }

    const T& Value() const {
        if (!is_initialized_) {
            throw BadOptionalAccess{};
        }
        return *ptr_;
    }

    void Reset() {
        if (ptr_ != nullptr) {
            ptr_->~T();
            ptr_ = nullptr;
        }

        is_initialized_ = false;
    }

    template <typename ... Ts>
    void Emplace(Ts&& ... ts) {
        if (ptr_ != nullptr) {
            ptr_->~T();
            ptr_ = nullptr;
        }
        ptr_ = new (data_) T(std::forward<Ts>(ts) ...);
        is_initialized_ = true;
    }

private:
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
    T* ptr_ = nullptr;

    template <typename Value>
    void Forward (Value&& value) {
        if (ptr_ == nullptr) {
            ptr_ = new (data_) T(std::forward<Value>(value));
        }
        else {
            *ptr_ = std::forward<Value>(value);
        }

        is_initialized_ = true;
    }
};