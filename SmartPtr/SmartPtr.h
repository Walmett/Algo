#pragma once

#include <string>

class WeakPtr;

struct Counter {
    size_t count{0};
    size_t weak_count{0};
};

class SharedPtr {
public:
    friend class WeakPtr;

    SharedPtr() = default;

    SharedPtr(std::string* ptr) : ptr_(ptr), counter_(ptr ? new Counter{1, 0} : nullptr) {
    }

    SharedPtr(const WeakPtr& ptr);

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), counter_(other.counter_) {
        if (ptr_ != nullptr) {
            ++counter_->count;
        }
    }

    SharedPtr(SharedPtr&& other) : ptr_(other.ptr_), counter_(other.counter_) {
        other.ptr_ = nullptr;
        other.counter_ = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        Swap(SharedPtr(other));
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }
        Swap(SharedPtr(std::move(other)));
        return *this;
    }

    void Reset(std::string* ptr) {
        Swap(SharedPtr(ptr));
    }

    std::string* Get() const {
        return ptr_;
    }

    std::string& operator*() const {
        return *ptr_;
    }

    std::string* operator->() const {
        return ptr_;
    }

    operator bool() const {
        return ptr_ != nullptr;
    }

    void Swap(SharedPtr&& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(counter_, other.counter_);
    }

    size_t UseCount() const {
        return counter_ ? counter_->count : 0;
    }

    ~SharedPtr() {
        if (counter_ != nullptr) {
            --counter_->count;
            if (counter_->count == 0) {
                delete ptr_;
                if (counter_->weak_count == 0) {
                    delete counter_;
                }
            }
        }
    }

private:
    std::string* ptr_{nullptr};
    Counter* counter_{nullptr};
};

class WeakPtr {
public:
    friend class SharedPtr;

    WeakPtr() = default;

    WeakPtr(const WeakPtr& ptr) : ptr_(ptr.ptr_), counter_(ptr.counter_) {
        if (ptr_ != nullptr) {
            ++counter_->weak_count;
        }
    }

    WeakPtr(WeakPtr&& ptr) : ptr_(ptr.ptr_), counter_(ptr.counter_) {
        ptr.ptr_ = nullptr;
        ptr.counter_ = nullptr;
    }

    WeakPtr& operator=(const WeakPtr& ptr) {
        if (this == &ptr) {
            return *this;
        }
        Swap(WeakPtr(ptr));
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& ptr) {
        if (this == &ptr) {
            return *this;
        }
        Swap(WeakPtr(std::move(ptr)));
        return *this;
    }

    WeakPtr(const SharedPtr& ptr) : ptr_(ptr.ptr_), counter_(ptr.counter_) {
        if (ptr_ != nullptr) {
            ++counter_->weak_count;
        }
    }

    WeakPtr& operator=(const SharedPtr& ptr) {
        Swap(WeakPtr(ptr));
        return *this;
    }

    SharedPtr Lock() const {
        return IsExpired() ? nullptr : SharedPtr(*this);
    }

    bool IsExpired() const {
        return ptr_ == nullptr || counter_ == nullptr || counter_->count == 0;
    }

    void Swap(WeakPtr&& ptr) {
        std::swap(ptr_, ptr.ptr_);
        std::swap(counter_, ptr.counter_);
    }

    size_t UseCount() const {
        return counter_ ? counter_->count : 0;
    }

    ~WeakPtr() {
        if (counter_ != nullptr) {
            --counter_->weak_count;
            if (counter_->count == 0 && counter_->weak_count == 0) {
                delete counter_;
            }
        }
    }

private:
    std::string* ptr_{nullptr};
    Counter* counter_{nullptr};
};

SharedPtr::SharedPtr(const WeakPtr& ptr) : ptr_(ptr.ptr_), counter_(ptr.counter_) {
    if (ptr.IsExpired()) {
        ptr_ = nullptr;
        counter_ = nullptr;
    } else if (ptr_ != nullptr) {
        ++counter_->count;
    }
}