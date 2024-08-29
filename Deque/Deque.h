#pragma once

#include <iostream>
#include <vector>

template<typename T, int blockSize = 6>
class Deque {
public:
    Deque() : Deque(0) {}
    Deque(int sz) : size_(sz) {
        blocks.resize(std::max(3, size_ * 3 / blockSize), nullptr);
        head_ = blocks.size() * blockSize / 2 - size_ / 2;
        tail_ = head_ + size_ - 1;
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i] = reinterpret_cast<T *>(new int8_t[blockSize * sizeof(T)]);
        }
    }
    ~Deque() {
        clear_();
    }
    Deque(const Deque &other) : size_(other.size_), head_(other.head_), tail_(other.tail_) {
        blocks.resize(other.blocks.size(), nullptr);
        int index = 0;
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i] = reinterpret_cast<T *>(new int8_t[blockSize * sizeof(T)]);
            try {
                for (int j = 0; j < blockSize; ++j) {
                    if (head_ <= index && index <= tail_) {
                        new(blocks[i] + j) T(other.blocks[i][j]);
                    }
                    ++index;
                }
            } catch (...) {
                for (size_t k = 0; k <= i; ++k) {
                    for (int j = 0; j < blockSize; ++j) {
                        (blocks[k] + j)->~T();
                    }
                    delete[] reinterpret_cast<int8_t *>(blocks[k]);
                }
                throw;
            }
        }
    }
    Deque(int sz, const T &value) : Deque(sz) {
        std::pair<int, int> pos = getPos(head_);
        while (sz--) {
            try {
                new(blocks[pos.first] + pos.second) T(value);
            } catch (...) {
                for (size_t k = 0; k < blocks.size(); ++k) {
                    for (int j = 0; j < blockSize; ++j) {
                        (blocks[k] + j)->~T();
                    }
                    delete[] reinterpret_cast<int8_t *>(blocks[k]);
                }
                throw;
            }
            if (pos.second + 1 == blockSize) {
                pos.second = 0;
                ++pos.first;
            } else {
                ++pos.second;
            }
        }
    }
    Deque &operator=(const Deque &other) {
        int cur_index;
        try {
            int new_cell_index = blocks.size();
            int free_space = blocks.size() - tail_ / blockSize - 1;
            blocks.resize(blocks.size() + std::max(0, int(other.blocks.size()) - free_space));
            for (size_t i = new_cell_index; i < blocks.size(); ++i) {
                blocks[i] = reinterpret_cast<T *>(new int8_t[blockSize * sizeof(T)]);
            }
            int start = tail_ + 1;
            cur_index = tail_;
            for (size_t i = 0; i < other.size(); ++i) {
                ++cur_index;
                std::pair<int, int> pos = getPos(cur_index);
                new(blocks[pos.first] + pos.second) T(other[i]);
            }
            std::pair<int, int> pos = getPos(head_);
            while (size_--) {
                (blocks[pos.first] + pos.second)->~T();
                if (pos.second + 1 == blockSize) {
                    pos.second = 0;
                    ++pos.first;
                } else {
                    ++pos.second;
                }
            }
            size_ = other.size_;
            head_ = start;
            tail_ = start + size_ - 1;
        } catch (...) {
            for (int i = tail_ + 1; i <= cur_index; ++i) {
                std::pair<int, int> pos = getPos(i);
                (blocks[pos.first] + pos.second)->~T();
            }
        }
        return *this;
    }
    T &operator[](int index) {
        return blocks[(head_ + index) / blockSize][(head_ + index) % blockSize];
    }
    const T &operator[](int index) const {
        return blocks[(head_ + index) / blockSize][(head_ + index) % blockSize];
    }
    T &at(int index) {
        if (size_ <= index) {
            throw std::out_of_range("out of range");
        }
        return (*this)[index];
    }
    const T &at(int index) const {
        if (size_ <= index) {
            throw std::out_of_range("out of range");
        }
        return (*this)[index];
    }
    void push_back(const T &value) {
        bool pass_rebuild = false;
        try {
            ++size_;
            if (tail_ + 2u > blocks.size() * blockSize) {
                rebuild_();
            }
            pass_rebuild = true;
            ++tail_;
            std::pair<int, int> pos = getPos(tail_);
            (blocks[pos.first] + pos.second)->~T();
            new(blocks[pos.first] + pos.second) T(value);
        } catch (...) {
            --size_;
            if (pass_rebuild) {
                std::pair<int, int> pos = getPos(tail_);
                (blocks[pos.first] + pos.second)->~T();
                --tail_;
            }
        }
    }
    void pop_back() {
        pop_back_();
    }
    void push_front(const T &value) {
        bool pass_rebuild = false;
        try {
            ++size_;
            if (head_ < 2) {
                rebuild_();
            }
            pass_rebuild = true;
            --head_;
            std::pair<int, int> pos = getPos(head_);
            (blocks[pos.first] + pos.second)->~T();
            new(blocks[pos.first] + pos.second) T(value);
        } catch(...) {
            --size_;
            if (pass_rebuild) {
                std::pair<int, int> pos = getPos(head_);
                (blocks[pos.first] + pos.second)->~T();
                ++head_;
            }
        }
    }
    void pop_front() {
        pop_front_();
    }
    size_t size() const {
        return size_;
    }
    bool empty() const {
        return size_ == 0;
    }
    template<bool IsConst>
    struct common_iterator {
        friend common_iterator<false>;

        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

    private:
        using T_ = std::conditional_t<IsConst, const T, T>;

        typename std::vector<T *>::const_iterator blockIter;
        int blockIndex;
        T_ *ptr;

        common_iterator(T_ *ptr_, typename std::vector<T *>::iterator blockIter_, int blockIndex_) :
                blockIter(blockIter_), blockIndex(blockIndex_), ptr(ptr_) {}
    public:
        common_iterator() : ptr(nullptr) {}
        common_iterator(const std::vector<T *> &blocks, int index) :
                blockIter(blocks.begin() + index / blockSize),
                blockIndex(index % blockSize),
                ptr(&blocks[index / blockSize][blockIndex]) {}
        ~common_iterator() {}
        common_iterator(const common_iterator &other)
                : blockIter(other.blockIter), blockIndex(other.blockIndex), ptr(other.ptr) {}
        common_iterator &operator=(const common_iterator &other) {
            blockIter = other.blockIter;
            blockIndex = other.blockIndex;
            ptr = other.ptr;
            return *this;
        }
        operator common_iterator<true>() {
            return common_iterator<true>(ptr, blockIter, blockIndex);
        }
        T_ &operator*() const {
            return *ptr;
        }
        T_ *operator->() const {
            return ptr;
        }
        common_iterator &operator++() {
            if (++blockIndex == blockSize) {
                blockIndex = 0;
                ++blockIter;
                ptr = &(*blockIter)[blockIndex];
            } else {
                ++ptr;
            }
            return *this;
        }
        common_iterator operator++(int) {
            common_iterator result = *this;
            ++*this;
            return result;
        }
        common_iterator &operator--() {
            if (--blockIndex == -1) {
                blockIndex = blockSize - 1;
                --blockIter;
                ptr = &(*blockIter)[blockIndex];
            } else {
                --ptr;
            }
            return *this;
        }
        common_iterator operator--(int) {
            common_iterator result = *this;
            --*this;
            return result;
        }
        common_iterator &operator+=(int n) {
            blockIndex += n % blockSize;
            if (blockIndex < 0) {
                blockIndex += blockSize;
                --blockIter;
            }
            if (blockSize <= blockIndex) {
                blockIndex -= blockSize;
                ++blockIter;
            }
            blockIter += n / blockSize;
            ptr = &(*blockIter)[blockIndex];
            return *this;
        }
        common_iterator operator+(int n) const {
            common_iterator result = *this;
            return result += n;
        }
        common_iterator &operator-=(int n) {
            return *this += (-n);
        }
        common_iterator operator-(int n) const {
            common_iterator result = *this;
            return result -= n;
        }
        bool operator==(const common_iterator &other) const {
            return (blockIter == other.blockIter) & (blockIndex == other.blockIndex);
        }
        bool operator!=(const common_iterator &other) const {
            return !(*this == other);
        }
        bool operator<(const common_iterator &other) const {
            if (blockIter != other.blockIter) {
                return blockIter < other.blockIter;
            }
            return blockIndex < other.blockIndex;
        }
        bool operator>(const common_iterator &other) const {
            return other < *this;
        }
        bool operator>=(const common_iterator &other) const {
            return !(*this < other);
        }
        bool operator<=(const common_iterator &other) const {
            return other >= *this;
        }
        int operator-(const common_iterator &other) const {
            return (blockIter - other.blockIter) * blockSize + (blockIndex - other.blockIndex);
        }
    };
    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(blocks, head_);
    }
    iterator end() {
        return iterator(blocks, tail_ + 1);
    }
    const_iterator begin() const {
        return const_iterator(blocks, head_);
    }
    const_iterator end() const {
        return const_iterator(blocks, tail_ + 1);
    }
    const_iterator cbegin() const {
        return const_iterator(blocks, head_);
    }
    const_iterator cend() const {
        return const_iterator(blocks, tail_ + 1);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(iterator(blocks, tail_ + 1));
    }
    reverse_iterator rend() {
        return reverse_iterator(iterator(blocks, head_));
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(iterator(blocks, tail_ + 1));
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(iterator(blocks, head_));
    }
    const_reverse_iterator rcbegin() const {
        return const_reverse_iterator(blocks, tail_ + 1);
    }
    const_reverse_iterator rcend() const {
        return const_reverse_iterator(blocks, head_);
    }

    void insert(iterator it, const T &value) {
        Deque save;
        try {
            save = *this;
        } catch (...) {
            throw;
        }
        try {
            insert_(it, value);
        } catch (...) {
            *this = save;
            throw;
        }
    }
    void erase(iterator it) {
        Deque save;
        try {
            save = *this;
        } catch (...) {
            throw;
        }
        try {
            erase_(it);
        } catch (...) {
            *this = save;
            throw;
        }
    }
private:
    int size_;
    int head_;
    int tail_;
    std::vector<T *> blocks;

    std::pair<int, int> getPos(int index) {
        return {index / blockSize, index % blockSize};
    }
    void rebuild_() {
        if (blocks.size() < size_ * 3u / blockSize) {
            blocks.resize(size_ * 3 / blockSize, nullptr);
        }
        std::pair<int, int> headPos = getPos(head_);
        std::pair<int, int> tailPos = getPos(tail_);
        int mid = (blocks.size() - tailPos.first + headPos.first - 1) / 2;
        int delta = mid - headPos.first;
        for (size_t i = tailPos.first + 1; i < blocks.size(); ++i) {
            if (blocks[i] == nullptr) {
                blocks[i] = reinterpret_cast<T *>(new int8_t[blockSize * sizeof(T)]);
            }
        }
        if (delta > 0) {
            for (int i = tailPos.first; i >= headPos.first; --i) {
                std::swap(blocks[i + delta], blocks[i]);
            }
        } else {
            for (int i = headPos.first; i <= tailPos.first; ++i) {
                std::swap(blocks[i + delta], blocks[i]);
            }
        }
        head_ = (headPos.first + delta) * blockSize + headPos.second;
        tail_ = (tailPos.first + delta) * blockSize + tailPos.second;
    }
    void pop_back_() {
        if (size_) {
            (blocks[tail_ / blockSize] + tail_ % blockSize)->~T();
            --size_;
            --tail_;
        }
    }
    void pop_front_() {
        if (size_) {
            (blocks[head_ / blockSize] + head_ % blockSize)->~T();
            --size_;
            ++head_;
        }
    }
    void insert_(iterator it, const T &value) {
        push_back(value);
        iterator cur = --end();
        while (cur != it) {
            iterator tmp = cur--;
            std::swap(*tmp, *cur);
        }
    }
    void erase_(iterator it) {
        while (it + 1 != end()) {
            iterator tmp = it++;
            std::swap(*tmp, *it);
        }
        pop_back();
    }
    void clear_() {
        std::pair<int, int> pos = getPos(head_);
        while (size_--) {
            (blocks[pos.first] + pos.second)->~T();
            if (pos.second + 1 == blockSize) {
                pos.second = 0;
                ++pos.first;
            } else {
                ++pos.second;
            }
        }
        for (size_t i = 0; i < blocks.size(); ++i) {
            delete[] reinterpret_cast<int8_t *>(blocks[i]);
        }
    }
};