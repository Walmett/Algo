#pragma once

#include <concepts>
#include <cstdlib>
#include <vector>
#include <array>
#include <iterator>


inline constexpr std::ptrdiff_t dynamic_stride = -1;

template <std::size_t extent = std::dynamic_extent, std::ptrdiff_t stride = 1>
class ExtentStrideStorage {
public:
    constexpr ExtentStrideStorage() = default;

    constexpr ExtentStrideStorage(const ExtentStrideStorage &storage) = default;

    constexpr std::size_t GetSize() const {
        return extent;
    }

    constexpr std::ptrdiff_t GetStride() const {
        return stride;
    }
};

template <std::ptrdiff_t stride>
class ExtentStrideStorage<std::dynamic_extent, stride> {
public:
    constexpr ExtentStrideStorage() = default;

    ExtentStrideStorage(std::size_t size) : size_(size) {}

    ExtentStrideStorage(const ExtentStrideStorage &storage) = default;

    std::size_t GetSize() const {
        return size_;
    }

    constexpr std::ptrdiff_t GetStride() const {
        return stride;
    }

protected:
    std::size_t size_ = 0;
};

template <std::size_t extent>
class ExtentStrideStorage<extent, dynamic_stride> {
public:
    constexpr ExtentStrideStorage() = default;

    ExtentStrideStorage(std::ptrdiff_t stride) : stride_(stride) {}

    ExtentStrideStorage(const ExtentStrideStorage &storage) = default;

    constexpr std::size_t GetSize() const {
        return extent;
    }

    std::ptrdiff_t GetStride() const {
        return stride_;
    }

protected:
    std::ptrdiff_t stride_ = 1;
};

template <>
class ExtentStrideStorage<std::dynamic_extent, dynamic_stride> {
public:
    constexpr ExtentStrideStorage() = default;

    ExtentStrideStorage(std::size_t size, std::ptrdiff_t stride) : size_(size), stride_(stride) {}

    ExtentStrideStorage(const ExtentStrideStorage &storage) = default;

    std::size_t GetSize() const {
        return size_;
    }

    std::ptrdiff_t GetStride() const {
        return stride_;
    }

protected:
    std::size_t size_ = 0;
    std::ptrdiff_t stride_ = 1;
};


template
        < class T
                , std::size_t extent = std::dynamic_extent
                , std::ptrdiff_t stride = 1
        >
class Slice : private ExtentStrideStorage<extent, stride> {
public:

    using value_type = std::remove_const_t<T>;
    using element_type = T;
    using size_type = std::size_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;


    Slice() = default;
    Slice(const Slice&) = default;
    Slice(Slice&&) = default;
    Slice& operator=(const Slice&) = default;
    Slice& operator=(Slice&&) = default;
    ~Slice() = default;

    template <std::contiguous_iterator It>
    Slice(It first, std::size_t count, std::ptrdiff_t skip)
            : data_(std::to_address(first)) {
        if constexpr (extent == std::dynamic_extent) {
            ExtentStrideStorage<extent, stride>::size_ = count;
        }
        if constexpr (stride == dynamic_stride) {
            ExtentStrideStorage<extent, stride>::stride_ = skip;
        }
    }

    template<typename U>
    Slice(std::vector<U>& vec) : ExtentStrideStorage<extent, stride>(vec.size()), data_(vec.data()) {
    }

    Slice(std::vector<T>& vec) : ExtentStrideStorage<extent, stride>(vec.size()), data_(vec.data()) {
    }

    Slice(std::array<T, extent>& array) : data_(array.data()) {
    }


    pointer Data() const {
        return data_;
    }

    constexpr size_type Size() const {
        return ExtentStrideStorage<extent, stride>::GetSize();
    }

    constexpr difference_type Stride() const {
        return ExtentStrideStorage<extent, stride>::GetStride();
    }

    constexpr bool Empty() const {
        return Size() == 0;
    }


    class Iterator : public std::random_access_iterator_tag {
    public:
        using iterator_category = std::random_access_iterator_tag;

        using value_type = value_type;
        using element_type = element_type;
        using size_type = size_type;
        using pointer = pointer;
        using const_pointer = const_pointer;
        using reference = reference;
        using const_reference = const_reference;
        using difference_type = difference_type;

        Iterator() = default;

        Iterator(const Iterator &other) = default;

        Iterator(pointer ptr, difference_type str) : ptr_(ptr), stride_(str) {
        }

        reference operator*() const {
            return *ptr_;
        }
        pointer operator->() const {
            return ptr_;
        }
        Iterator& operator++() {
            ptr_ += stride_;
            return *this;
        }
        Iterator operator++(int) {
            Iterator prev = *this;
            ++(*this);
            return prev;
        }
        Iterator& operator--() {
            ptr_ -= stride_;
            return *this;
        }
        Iterator operator--(int) {
            Iterator prev = *this;
            --(*this);
            return prev;
        }
        Iterator& operator+=(difference_type count) {
            ptr_ += count * stride_;
            return *this;
        }
        Iterator& operator-=(difference_type count) {
            return *this += -count;
        }
        Iterator operator+(difference_type rhs) const {
            return {ptr_ + rhs * stride_, stride_};
        }
        friend Iterator operator+(difference_type lhs, const Iterator rhs) {
            return Iterator(rhs.ptr_ + lhs * rhs.stride_, rhs.stride_);
        }
        Iterator operator-(difference_type rhs) const {
            return {ptr_ - rhs * stride_, stride_};
        }
        difference_type operator-(const Iterator& other) const {
            return (ptr_ - other.ptr_) / stride_;
        }
        reference operator[](difference_type index) const {
            return *(ptr_ + index * stride_);
        }

        bool operator==(const Iterator& rhs) const {
            return ptr_ == rhs.ptr_;
        }
        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }
        bool operator<(const Iterator& rhs) const {
            return ptr_ < rhs.ptr_;
        }
        bool operator>(const Iterator& rhs) const {
            return rhs < *this;
        }
        bool operator<=(const Iterator& rhs) const {
            return !(*this > rhs);
        }
        bool operator>=(const Iterator& rhs) const {
            return !(*this < rhs);
        }

    private:
        pointer ptr_{nullptr};
        difference_type stride_{1};
    };

    using iterator = Iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;

    iterator begin() const {
        return {Data(), Stride()};
    }

    iterator end() const {
        return {Data() + Size() * Stride(), Stride()};
    }

    reverse_iterator rbegin() const {
        return end();
    }

    reverse_iterator rend() const {
        return begin();
    }

    reference operator[](std::size_t index) const {
        return *(Data() + index * Stride());
    }

    reference Front() const {
        return *Data();
    }

    reference Back() const {
        return *(Data() + (Size() - 1) * Stride());
    }


    Slice<T, std::dynamic_extent, stride> First(std::size_t count) const {
        return {Data(), count, Stride()};
    }

    template <std::size_t count>
    Slice<T, count, stride> First() const {
        return {Data(), count, Stride()};
    }

    Slice<T, std::dynamic_extent, stride> Last(std::size_t count) const {
        return {Data() + (Size() - count) * stride, count, Stride()};
    }

    template <std::size_t count>
    Slice<T, count, stride> Last() const {
        return {Data() + (Size() - count) * stride, count, Stride()};
    }

    Slice<T, std::dynamic_extent, stride> DropFirst(std::size_t count) const {
        return {Data() + count * stride, Size() - count, Stride()};
    }

    template <std::size_t count>
    Slice<T, std::dynamic_extent, stride> DropFirst() const {
        return {Data() + count * stride, Size() - count, Stride()};
    }

    Slice<T, std::dynamic_extent, stride> DropLast(std::size_t count) const {
        return {Data(), Size() - count, Stride()};
    }

    template <std::size_t count>
    Slice<T, std::dynamic_extent, stride> DropLast() const {
        return {Data(), Size() - count, Stride()};
    }

    Slice<T, std::dynamic_extent, dynamic_stride> Skip(std::ptrdiff_t skip) const {
        return {Data(), (Size() + skip - 1) / skip, Stride() * skip};
    }

    template <std::ptrdiff_t skip>
    Slice<T, extent == std::dynamic_extent ? extent : (extent + skip - 1) / skip, stride == dynamic_stride ? stride : stride * skip> Skip() const {
        return {Data(), (Size() + skip - 1) / skip, Stride() * skip};
    }


    template <typename U, std::size_t ext, std::ptrdiff_t str>
    requires (std::is_same_v<T, U> || std::is_same_v<const T, U>)
    operator Slice<U, ext, str>() const {
        return {Data(), Size(), Stride()};
    }

    template<typename U, std::size_t ext, std::ptrdiff_t str>
    bool operator==(const Slice<U, ext, str> &rhs) const {
        auto lhs_it = begin();
        auto rhs_it = rhs.begin();
        while (lhs_it != end() && rhs_it != rhs.end() && *lhs_it == *rhs_it) {
            ++lhs_it;
            ++rhs_it;
        }
        return lhs_it == end() && rhs_it == rhs.end();
    }

private:
    T* data_;
};


template<typename Iterator>
Slice(Iterator first, std::size_t size, std::ptrdiff_t stride)
-> Slice<typename std::iterator_traits<Iterator>::value_type, std::dynamic_extent, dynamic_stride>;