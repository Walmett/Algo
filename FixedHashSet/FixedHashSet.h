#pragma once

#include <vector>
#include <cstdint>
#include <random>
#include <optional>
#include <algorithm>

constexpr uint64_t kPrime = 5310000001;

class LinearHashFunction {
public:
    LinearHashFunction() : a_(1), b_(0) {}

    LinearHashFunction(int64_t a, int64_t b) : a_(a), b_(b) {}

    uint64_t operator()(int number) const {
        return ((a_ * (kPrime + number)) + b_) % kPrime;
    }

private:
    uint64_t a_;
    uint64_t b_;
};

template<class RandomGenerator>
LinearHashFunction LinearHashFunctionGenerator(RandomGenerator &generator) {
    std::uniform_int_distribution<int64_t> distribution(1, kPrime - 1);
    return LinearHashFunction{distribution(generator), distribution(generator)};
}

std::vector<std::vector<int>> GetBuckets(const std::vector<int> &numbers,
                                         LinearHashFunction hash_function,
                                         size_t capacity) {
    std::vector<std::vector<int>> buckets(capacity);
    for (int number: numbers) {
        buckets[hash_function(number) % capacity].push_back(number);
    }
    return buckets;
}

bool CheckBucketsSizes(const std::vector<std::vector<int>> &buckets) {
    constexpr size_t kSizeCoefficient = 4;
    uint64_t sqr_sum = 0;
    for (const auto &bucket: buckets) {
        sqr_sum += static_cast<uint64_t>(bucket.size()) * bucket.size();
    }
    return sqr_sum <= kSizeCoefficient * buckets.size();
}

bool CheckAnyCollisions(const std::vector<std::vector<int>> &buckets) {
    return std::all_of(buckets.begin(), buckets.end(),
                       [](const auto &bucket) { return bucket.size() <= 1; });
}

template<class Predicate>
auto GenerateHashFunction(const std::vector<int> &numbers, size_t capacity, Predicate predicate) {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    LinearHashFunction hash_function;
    std::vector<std::vector<int>> buckets;
    do {
        hash_function = LinearHashFunctionGenerator(generator);
        buckets = GetBuckets(numbers, hash_function, capacity);
    } while (!predicate(buckets));
    return hash_function;
}

class HashTable {
public:
    HashTable() = default;

    void
    Initialize(const std::vector<int> &numbers, size_t capacity) {
        hash_table_.clear();
        hash_table_.resize(capacity);
        hash_function_ = GenerateHashFunction(numbers, capacity, CheckAnyCollisions);
        for (int number: numbers) {
            hash_table_[hash_function_(number) % hash_table_.size()] = number;
        }
    }

    bool Contains(int number) const {
        if (hash_table_.empty()) {
            return false;
        }
        size_t index = hash_function_(number) % hash_table_.size();
        return hash_table_[index] && *hash_table_[index] == number;
    }

private:
    LinearHashFunction hash_function_;
    std::vector<std::optional<int>> hash_table_;
};

class FixedHashSet {
public:
    FixedHashSet() = default;

    void Initialize(const std::vector<int> &numbers) {
        first_hash_table_.clear();
        if (numbers.empty()) {
            return;
        }
        first_hash_table_.resize(numbers.size());
        hash_function_ = GenerateHashFunction(numbers, first_hash_table_.size(), CheckBucketsSizes);
        auto buckets = GetBuckets(numbers, hash_function_, first_hash_table_.size());
        for (size_t i = 0; i < buckets.size(); ++i) {
            first_hash_table_[i].Initialize(buckets[i], buckets[i].size() * buckets[i].size());
        }
    }

    bool Contains(int number) const {
        if (first_hash_table_.empty()) {
            return false;
        }
        return first_hash_table_[hash_function_(number) % first_hash_table_.size()].Contains(
                number);
    }

private:
    LinearHashFunction hash_function_;
    std::vector<HashTable> first_hash_table_;
};
