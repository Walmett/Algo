#pragma once

#include <algorithm>
#include <cstddef>

class ListHook {
public:
    bool IsLinked() const {
        return prev_ != nullptr && next_ != nullptr && prev_ != this && next_ != this;
    }
    void Unlink() {
        if (IsLinked()) {
            prev_->next_ = next_;
            next_->prev_ = prev_;
            prev_ = this;
            next_ = this;
        }
    }

    ListHook(const ListHook&) = delete;
    ListHook& operator=(const ListHook&) = delete;

protected:
    ListHook() : prev_(nullptr), next_(nullptr) {
    }

    ~ListHook() {
        Unlink();
    }

    void LinkBefore(ListHook* other) {
        prev_ = other->prev_;
        next_ = other;
        prev_->next_ = this;
        next_->prev_ = this;
    }

    template <class T>
    friend class List;

    ListHook* prev_;
    ListHook* next_;
};

template <class T>
class List {
public:
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() = default;
        Iterator(const Iterator& other) : node_(other.node_) {
        }
        Iterator(Iterator&& other) : node_(std::move(other.node_)) {
        }
        Iterator(ListHook* node) : node_(node) {
        }

        Iterator& operator=(const Iterator& other) {
            node_ = other.node_;
            return *this;
        }

        Iterator& operator=(Iterator&& other) {
            node_ = std::move(other.node_);
            return *this;
        }

        T& operator*() const {
            return *static_cast<T*>(node_);
        }

        T* operator->() const {
            return static_cast<T*>(node_);
        }

        Iterator& operator++() {
            node_ = node_->next_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator result = *this;
            node_ = node_->next_;
            return result;
        }

        Iterator& operator--() {
            node_ = node_->prev_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator result = *this;
            node_ = node_->prev_;
            return result;
        }

        bool operator==(const Iterator& rhs) const {
            return node_ == rhs.node_;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

    private:
        ListHook* node_;
    };

    List() : dummy_() {
        dummy_.prev_ = &dummy_;
        dummy_.next_ = &dummy_;
    }

    List(const List&) = delete;

    List(List&& other) : dummy_() {
        dummy_.prev_ = other.dummy_.prev_;
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_->next_ = &dummy_;
        dummy_.next_->prev_ = &dummy_;
        other.dummy_.prev_ = &other.dummy_;
        other.dummy_.next_ = &other.dummy_;
    }

    ~List() {
        while (!IsEmpty()) {
            PopFront();
        }
    }

    List& operator=(const List&) = delete;

    List& operator=(List&& other) {
        while (!IsEmpty()) {
            PopFront();
        }
        dummy_.prev_ = other.dummy_.prev_;
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_->next_ = &dummy_;
        dummy_.next_->prev_ = &dummy_;
        other.dummy_.prev_ = &other.dummy_;
        other.dummy_.next_ = &other.dummy_;
        return *this;
    }

    bool IsEmpty() const {
        return Size() == 0;
    }

    size_t Size() const {
        size_t size = 0;
        for (auto it = Begin(); it != End(); ++it) {
            ++size;
        }
        return size;
    }

    void PushBack(T* elem) {
        elem->Unlink();
        elem->LinkBefore(&dummy_);
    }
    void PushFront(T* elem) {
        elem->Unlink();
        elem->LinkBefore(dummy_.next_);
    }

    T& Front() {
        return *Begin();
    }

    const T& Front() const {
        return *Begin();
    }

    T& Back() {
        return *(--End());
    }

    const T& Back() const {
        return *(--End());
    }

    void PopBack() {
        dummy_.prev_->Unlink();
    }

    void PopFront() {
        dummy_.next_->Unlink();
    }

    Iterator Begin() const {
        return {dummy_.next_};
    }

    Iterator End() const {
        return {dummy_.next_->prev_};
    }

    Iterator IteratorTo(T* element) {
        return {element};
    }

private:
    ListHook dummy_;
};

template <class T>
List<T>::Iterator begin(List<T>& list) {
    return list.Begin();
}

template <class T>
List<T>::Iterator end(List<T>& list) {
    return list.End();
}
