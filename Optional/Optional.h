#pragma once

#include <cstdint>
#include <utility>
#include <stdexcept>

template <class T>
class Optional {
public:
    Optional() {
    }

    Optional(const T& value) {
        has_value_ = true;
        new (reinterpret_cast<T*>(data_)) T(value);
    }

    Optional(T&& value) {
        has_value_ = true;
        new (reinterpret_cast<T*>(data_)) T(std::move(value));
    }

    Optional(const Optional& other) {
        if (other.has_value_) {
            has_value_ = true;
            new (reinterpret_cast<T*>(data_)) T(*reinterpret_cast<const T*>(other.data_));
        }
    }

    Optional(Optional&& other) {
        if (other.has_value_) {
            has_value_ = true;
            new (reinterpret_cast<T*>(data_)) T(std::move(*reinterpret_cast<T*>(other.data_)));
        }
    }

    ~Optional() {
        Reset();
    }

    Optional& operator=(const Optional& other) {
        if (this == &other) {
            return *this;
        }
        Optional<T> temp = std::move(*this);
        if (other.has_value_) {
            has_value_ = true;
            new (reinterpret_cast<T*>(data_)) T(*reinterpret_cast<const T*>(other.data_));
        } else {
            has_value_ = false;
        }
        return *this;
    }

    Optional& operator=(Optional&& other) {
        if (this == &other) {
            return *this;
        }
        Optional<T> temp = std::move(*this);
        if (other.has_value_) {
            has_value_ = true;
            new (reinterpret_cast<T*>(data_)) T(std::move(*reinterpret_cast<T*>(other.data_)));
        } else {
            has_value_ = false;
        }
        return *this;
    }

    T& operator*() & {
        return Value();
    }

    const T& operator*() const& {
        return Value();
    }

    T&& operator*() && {
        return std::move(Value());
    }

    T* operator->() {
        return reinterpret_cast<T*>(data_);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(data_);
    }

    T& Value() & {
        if (!has_value_) {
            throw std::out_of_range{""};
        }
        return *reinterpret_cast<T*>(data_);
    }

    const T& Value() const& {
        if (!has_value_) {
            throw std::out_of_range{""};
        }
        return *reinterpret_cast<const T*>(data_);
    }

    T&& Value() && {
        if (!has_value_) {
            throw std::out_of_range{""};
        }
        return std::move(*reinterpret_cast<T*>(data_));
    }

    bool HasValue() const {
        return has_value_;
    }

    void Reset() {
        if (has_value_) {
            has_value_ = false;
            reinterpret_cast<T*>(data_)->~T();
        }
    }

private:
    alignas(alignof(T)) int8_t data_[sizeof(T)]{};
    bool has_value_{false};
};
