#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include <exception>

#include <iostream>

template <class T>
concept NotAny = !std::same_as<std::remove_cvref_t<T>, class Any>;

template <typename T>
concept CheckConceptHaveField = requires { T().x; };

class Any {
public:
    Any() : ptr_(nullptr) {
    }

    template <NotAny T>
    Any(T&& value)
            : ptr_(new ObjectImpl<std::remove_const_t<std::remove_reference_t<T>>>(
            std::forward<T>(value))) {
    }

    Any(const Any& other) {
        if (other.ptr_ != nullptr) {
            ptr_ = other.ptr_->Copy();
        }
    }

    Any(Any&& other) : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    Any& operator=(const Any& other) {
        if (this == &other) {
            return *this;
        }
        if (other.ptr_ != nullptr) {
            auto* ptr_to_delete = ptr_;
            ptr_ = other.ptr_->Copy();
            delete ptr_to_delete;
        } else {
            Clear();
        }
        return *this;
    }

    Any& operator=(Any&& other) {
        if (this == &other) {
            return *this;
        }
        delete ptr_;
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    }

    ~Any() {
        delete ptr_;
    }

    bool Empty() const {
        return ptr_ == nullptr;
    }

    void Clear() {
        delete ptr_;
        ptr_ = nullptr;
    }
    void Swap(Any& other) {
        std::swap(ptr_, other.ptr_);
    }

    template <class T>
    const T& GetValue() const {
        if (ptr_ == nullptr) {
            throw std::bad_cast();
        }
        if (ptr_->TypeInfo() != typeid(T)) {
            throw std::bad_cast();
        }
        return dynamic_cast<ObjectImpl<std::remove_const_t<std::remove_reference_t<T>>>*>(ptr_)
                ->GetObject();
    }

private:
    class Object {
    public:
        Object() = default;
        virtual ~Object() = default;

        virtual const std::type_info& TypeInfo() const = 0;
        virtual Object* Copy() const = 0;
    };

    template <class T>
    class ObjectImpl : public Object {
    public:
        template <class U>
        ObjectImpl(U&& value) : value_(std::forward<U>(value)) {
        }

        template <CheckConceptHaveField U>
        ObjectImpl(U&& value) : value_(value) {
        }

        const T& GetObject() const {
            return value_;
        }

        const std::type_info& TypeInfo() const override {
            return typeid(T);
        }

        Object* Copy() const override {
            return new ObjectImpl<T>(std::forward<T>(value_));
        }

    private:
        mutable std::remove_const_t<std::remove_reference_t<T>> value_;
    };

    Object* ptr_{nullptr};
};
