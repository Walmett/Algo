#pragma once

#include <iostream>
#include <vector>
#include <cassert>

template<typename T>
class Heap {
public:
    Heap(std::vector<T> &arr) {
        heap = arr;
        HeapBuild();
    }
    ~Heap() {}
    Heap(const Heap &) = delete;
    Heap &operator=(const Heap &) = delete;

    void update(const T &value) { HeapUpdate(value); }
    void getSortedElements(std::vector<T> &arr) { HeapGetSortedElements(arr); }

private:
    std::vector<T> heap;

    void HeapSiftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent] >= heap[index]) {
                return;
            }
            std::swap(heap[index], heap[parent]);
            index = parent;
        }
    }
    void HeapSiftDown(int index) {
        while (index * 2 + 1 < heap.size()) {
            int left = index * 2 + 1,
                    right = index * 2 + 2;
            int next = index;
            if (heap[left] > heap[next]) {
                next = left;
            }
            if (right < heap.size() && heap[right] > heap[next]) {
                next = right;
            }
            if (next == index) {
                return;
            }
            std::swap(heap[next], heap[index]);
            index = next;
        }
    }
    void HeapBuild() {
        for (int i = heap.size() / 2 - 1; i >= 0; i--) {
            HeapSiftDown(i);
        }
    }
    void HeapInsert(T value) {
        heap.push_back(value);
        HeapSiftUp(heap.size() - 1);
    }
    T HeapGetMax() {
        assert(heap.size() > 0);
        return heap[0];
    }
    void HeapExtractMax() {
        assert(heap.size() > 0);
        std::swap(heap.front(), heap.back());
        heap.pop_back();
        HeapSiftDown(0);
    }
    void HeapUpdate(T value) {
        if (value < HeapGetMax()) {
            HeapExtractMax();
            HeapInsert(value);
        }
    }
    void HeapGetSortedElements(std::vector<T> &arr) {
        for (int i = arr.size() - 1; i >= 0; i--) {
            arr[i] = HeapGetMax();
            HeapExtractMax();
        }
    }
};
