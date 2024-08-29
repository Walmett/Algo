#pragma once

#include <iostream>
#include <random>

template<class Iterator, class RandomGenerator>
Iterator SelectPivot(Iterator begin, Iterator end, RandomGenerator& generator) {
    std::uniform_int_distribution<> index_distribution(0, end - begin - 1);
    Iterator pivot = begin + index_distribution(generator);
    return pivot;
}

template<class Iterator, class Comparator, class RandomGenerator>
Iterator Partition(Iterator begin, Iterator end, Comparator compare, RandomGenerator& generator) {
    Iterator pivot = SelectPivot(begin, end, generator);
    Iterator right = end - 1;
    std::swap(*pivot, *right);
    for (Iterator it = begin; it < right; ++it) {
        if (compare(*it, *right)) {
            std::swap(*it, *begin);
            ++begin;
        }
    }
    std::swap(*begin, *right);
    right = begin + 1;
    while (right < end && !compare(*right, *begin) && !compare(*begin, *right)) {
        ++right;
    }
    return begin + (right - begin) / 2;
}

template<class Iterator, class Comparator, class RandomGenerator>
void QuickSort(Iterator begin, Iterator end, Comparator compare, RandomGenerator& generator) {
    if (begin == end) {
        return;
    }
    auto mid = Partition(begin, end, compare, generator);
    QuickSort(begin, mid, compare, generator);
    QuickSort(mid + 1, end, compare, generator);
}

template <class Iterator, class Comparator>
void Sort(Iterator begin, Iterator end, Comparator compare) {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    QuickSort(begin, end, compare, generator);
}