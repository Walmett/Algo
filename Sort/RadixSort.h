#pragma once

#include <iostream>
#include <cstring>

int GetByte(uint64_t value, int p) {
    return (value >> (8 * p)) % 256;
}

void ByteSort(uint64_t *arr, int l, int r, int p) {
    int n = r - l + 1;
    auto *a = new uint64_t[n];
    std::memcpy(a, arr + l, sizeof(uint64_t) * n);
    int *cnt = new int[256];
    for (int i = 0; i < 256; ++i) {
        cnt[i] = 0;
    }
    for (int i = 0; i < n; ++i) {
        ++cnt[GetByte(a[i], p)];
    }
    for (int i = 1; i < 256; ++i) {
        cnt[i] += cnt[i - 1];
    }
    for (int i = n - 1; i >= 0; --i) {
        arr[--cnt[GetByte(a[i], p)] + l] = a[i];
    }
    delete[] a;
    delete[] cnt;
}

void RadixSort(uint64_t *arr, int l, int r) {
    for (int i = 0; i < 8; ++i) {
        ByteSort(arr, l, r, i);
    }
}