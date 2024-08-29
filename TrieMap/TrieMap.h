#pragma once

#include <iostream>
#include <vector>

#include <map>
#include <unordered_map>
#include <windows.h>
#include <random>

// Map <UIntMax, UIntMax - 1>; on average 384 bites for
template <template <typename> class allocator = std::allocator>
class TrieMapUInt {
    static const int p = 4;
    static const int mod = (1 << p) - 1;
    static const int iterations = sizeof(unsigned int) * 8 / p - 1;
public:

    TrieMapUInt() : nodes(1) {}

    void set(unsigned int key, unsigned int value) {
        unsigned int current_node = 0;
        for (int i = 0; i != iterations; ++i) {
            unsigned int next_node = nodes[current_node].next[key & mod];
            if (next_node == 0) {
                nodes[current_node].next[key & mod] = next_node = create_node();
            }
            current_node = next_node;
            key >>= p;
        }
        nodes[current_node].next[key] = value + 1;
    }

    void erase(unsigned int key) {
        unsigned int current_node = 0;
        for (int i = 0; i != iterations; ++i) {
            unsigned int next_node = nodes[current_node].next[key & mod];
            if (next_node == 0) {
                return;
            }
            current_node = next_node;
            key >>= p;
        }
        nodes[current_node].next[key] = 0;
    }

    unsigned int get(unsigned int key) {
        unsigned int current_node = 0;
        for (int i = 0; i != iterations; ++i) {
            unsigned int next_node = nodes[current_node].next[key & mod];
            if (next_node == 0) {
                return -1;
            }
            current_node = next_node;
            key >>= p;
        }
        return nodes[current_node].next[key] - 1;
    }

    bool has(unsigned int key) {
        return get(key) != -1;
    }


private:
    struct node {
        unsigned int next[1 << p]{};
    };

    unsigned int create_node() {
        unsigned int node_id = nodes.size();
        nodes.emplace_back();
        return node_id;
    }

    std::vector<node, allocator<node>> nodes;
};

std::mt19937 rnd(42);

void test_trie(int n) {
    TrieMapUInt<> map;
    for (int i = 0; i < n; ++i) {
        map.set(rnd(), rnd());
    }
    for (int i = 0; i < n; ++i) {
        map.get(rnd());
    }
}

void test_map(int n) {
    std::unordered_map<unsigned int, unsigned int> map;
    for (int i = 0; i < n; ++i) {
        map.insert({rnd(), rnd()});
    }
    for (int i = 0; i < n; ++i) {
        map[rnd()];
    }
}

int main() {
    long long current_time = GetTickCount();

    //test_map(1000000);
    test_trie(1000000);

    std::cout << GetTickCount() - current_time;
}