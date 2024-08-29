#pragma once

#include <iterator>
#include <memory>
#include <array>

#include <iostream>

template <class T>
class ImmutableVector {
    constexpr static int kSize = 1 << 5;

public:
    struct Node {
        T value{};
        std::array<std::shared_ptr<Node>, kSize> next{};

        Node() = default;

        Node(const T& value) : value(value) {
        }

        Node(Node& other) : value(other.value), next(other.next) {
        }
    };

    ImmutableVector() {
    }

    ImmutableVector(std::shared_ptr<Node> ptr, size_t size) : root_(std::move(ptr)), size_(size) {
    }

    ImmutableVector(const ImmutableVector& other) : root_(other.root_), size_(other.size_) {
    }

    ImmutableVector& operator=(const ImmutableVector&) = delete;

    explicit ImmutableVector(size_t count, const T& value = {}) {
        for (size_t i = 0; i < count; ++i) {
            ImmutableVector next = PushBack(value);
            root_ = std::move(next.root_);
            size_ = next.size_;
        }
    }

    template <class Iterator>
    ImmutableVector(Iterator first, Iterator last) {
        for (; first != last; ++first) {
            ImmutableVector next = PushBack(*first);
            root_ = std::move(next.root_);
            size_ = next.size_;
        }
    }

    ImmutableVector(std::initializer_list<T> l) : ImmutableVector<T>(l.begin(), l.end()) {
    }

    ImmutableVector Set(size_t index, const T& value) const {
        if (!root_) {
            return {std::make_shared<Node>(value), 1};
        }
        std::shared_ptr<Node> result = std::make_shared<Node>(*root_);
        ;
        std::shared_ptr<Node> current = result;
        while (index != 0) {
            std::shared_ptr<Node>& next = current->next[index & (kSize - 1)];
            if (next || index / kSize != 0) {
                if (next) {
                    next = std::make_shared<Node>(*next);
                } else {
                    next = std::make_shared<Node>();
                }
            } else {
                next = std::make_shared<Node>(value);
                return {result, size_ + 1};
            }
            current = next;
            index /= kSize;
        }
        current->value = value;
        return {result, size_};
    }

    const T& Get(size_t index) const {
        std::shared_ptr<Node> current = root_;
        while (index != 0) {
            current = current->next[index & (kSize - 1)];
            index /= kSize;
        }
        return current->value;
    }

    ImmutableVector PushBack(const T& value) const {
        return Set(size_, value);
    }

    ImmutableVector PopBack() const {
        if (size_ == 1) {
            return {};
        }
        size_t index = size_ - 1;
        std::shared_ptr<Node> result = std::make_shared<Node>(*root_);
        std::shared_ptr<Node> current = result;
        while (index / kSize != 0) {
            auto& next = current->next[index & (kSize - 1)];
            next = std::make_shared<Node>(*next);
            current = next;
            index /= kSize;
        }
        current->next[index] = nullptr;
        return {result, size_ - 1};
    }

    size_t Size() const {
        return size_;
    }

private:
    std::shared_ptr<Node> root_{};
    size_t size_{0};
};

template <class Iterator>
ImmutableVector(Iterator, Iterator) -> ImmutableVector<std::iter_value_t<Iterator>>;
