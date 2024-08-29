#pragma once

#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>

std::mt19937_64 rnd(std::chrono::high_resolution_clock::now().time_since_epoch().count());

template<typename T>
class TreapSet {
public:
    TreapSet() {
        root = nullptr;
    }

    ~TreapSet() {
        clear_(root);
    }

    TreapSet(const TreapSet &other) {
        copy_(root, other.root);
    }

    TreapSet &operator=(const TreapSet &other) {
        copy_(root, other.root);
        return *this;
    }

    T &operator[](int index) {
        return find_by_order_(root, index)->key;
    }

    T *find_by_order(int index) {
        node *t = find_by_order_(root, index);
        if (t == nullptr) {
            return nullptr;
        }
        return &t->key;
    }

    T *next(const T &key) {
        node *t = next_(root, key);
        if (t == nullptr) {
            return nullptr;
        }
        return &t->key;
    }

    T *prev(const T &key) {
        node *t = prev_(root, key);
        if (t == nullptr) {
            return nullptr;
        }
        return &t->key;
    }

    T *array() {
        if (root != nullptr) {
            T *arr = new T[root->size];
            int ii = -1;
            array_(root, arr, ii);
            return arr;
        }
        return nullptr;
    }

    int size() {
        if (root == nullptr) {
            return 0;
        }
        return root->size;
    }

    void clear() {
        clear_(root);
        root = nullptr;
    }

    void output() {
        output_(root);
        std::cout << "\n";
    }

    void build(const T *arr, int size) {
        clear_(root);
        root = build_(arr, size);
    }

    bool empty() { return root == nullptr; }

    int order_of_key(const T &key) { return order_of_key_(root, key); }

    void unite(const TreapSet &other) { root = unite_(root, other.root); }

    void insert(const T &key) { insert_(key); }

    void erase(const T &key) { erase_(root, key); }

    bool find(const T &key) { return find_(root, key) != nullptr; }

    T &min() { return min_(root)->key; }

    T &max() { return max_(root)->key; }

    void erase_min() { erase_min_(root); }

    void erase_max() { erase_max_(root); }

private:
    struct node {
        T key;
        uint64_t prior;
        int size;
        node *left;
        node *right;

        node() {};

        node(const T &key_, int64_t rnd_)
                : key(key_), prior(rnd_), size(1), left(nullptr), right(nullptr) {}

        explicit node(const node *x)
                : key(x->key), prior(x->prior), size(x->size), left(nullptr), right(nullptr) {}
    };

    node *root = nullptr;

    void update_size(node *&cur) {
        if (cur != nullptr) {
            cur->size = 1;
            if (cur->left != nullptr) {
                cur->size += cur->left->size;
            }
            if (cur->right != nullptr) {
                cur->size += cur->right->size;
            }
        }
    }

    void split(node *cur, const T &key, node *&l, node *&r) {
        if (cur == nullptr) {
            l = r = nullptr;
            return;
        }
        if (key < cur->key) {
            split(cur->left, key, l, cur->left);
            r = cur;
        } else {
            split(cur->right, key, cur->right, r);
            l = cur;
        }
        update_size(cur);
    }

    void merge(node *&cur, node *l, node *r) {
        if (l == nullptr) {
            cur = r;
            return;
        }
        if (r == nullptr) {
            cur = l;
            return;
        }
        if (r->prior < l->prior) {
            merge(l->right, l->right, r);
            cur = l;
        } else {
            merge(r->left, l, r->left);
            cur = r;
        }
        update_size(cur);
    }

    void heapify(node *cur) {
        if (cur == nullptr) {
            return;
        }
        node *max = cur;
        if (cur->left != nullptr && max->prior < cur->left->prior) {
            max = cur->left;
        }
        if (cur->right != nullptr && max->prior < cur->right->prior) {
            max = cur->right;
        }
        if (max != cur) {
            std::swap(cur->prior, max->prior);
            heapify(max);
        }
    }

    node *build_(const T *arr, int size) {
        if (!size) {
            return nullptr;
        }
        int size_div = size >> 1;
        node *cur = new node(arr[size_div], rnd());
        cur->left = build_(arr, size_div);
        cur->right = build_(arr + size_div + 1, size - size_div - 1);
        heapify(cur);
        update_size(cur);
        return cur;
    }

    node *find_(node *cur, const T &key) {
        while (cur != nullptr) {
            if (key < cur->key) {
                cur = cur->left;
            } else if (key == cur->key) {
                return cur;
            } else {
                cur = cur->right;
            }
        }
        return nullptr;
    }

    void insert_node(node *&cur, node *it) {
        if (cur == nullptr) {
            cur = it;
        } else if (cur->prior < it->prior) {
            split(cur, it->key, it->left, it->right);
            cur = it;
        } else if (it->key < cur->key) {
            insert_node(cur->left, it);
        } else {
            insert_node(cur->right, it);
        }
        update_size(cur);
    }

    void insert_(const T &key) {
        if (find_(root, key) == nullptr) {
            node *it = new node(key, rnd());
            insert_node(root, it);
        }
    }

    void erase_(node *&cur, const T &key) {
        if (cur != nullptr) {
            if (key == cur->key) {
                node *t = cur;
                merge(cur, cur->left, cur->right);
                delete t;
            } else {
                if (key < cur->key) {
                    erase_(cur->left, key);
                } else {
                    erase_(cur->right, key);
                }
            }
            update_size(cur);
        }
    }

    node *find_by_order_(node *cur, int index) {
        while (cur != nullptr) {
            int left_size = 0;
            if (cur->left != nullptr) {
                left_size = cur->left->size;
            }
            if (index < left_size) {
                cur = cur->left;
            } else {
                if (index == left_size) {
                    break;
                }
                index -= left_size + 1;
                cur = cur->right;
            }
        }
        return cur;
    }

    int order_of_key_(node *cur, const T &key) {
        int ans = 0;
        while (cur != nullptr) {
            if (key < cur->key) {
                cur = cur->left;
            } else {
                if (cur->left != nullptr) {
                    ans += cur->left->size;
                }
                if (key == cur->key) {
                    return ans;
                }
                ++ans;
                cur = cur->right;
            }
        }
        return ans;
    }

    node *next_(node *cur, const T &key) {
        node *ans = nullptr;
        while (cur != nullptr) {
            if (key < cur->key) {
                ans = cur;
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        return ans;
    }

    node *prev_(node *cur, const T &key) {
        node *ans = nullptr;
        while (cur != nullptr) {
            if (cur->key < key) {
                ans = cur;
                cur = cur->right;
            } else {
                cur = cur->left;
            }
        }
        return ans;
    }

    node *min_(node *cur) {
        while (cur->left != nullptr) {
            cur = cur->left;
        }
        return cur;
    }

    node *max_(node *cur) {
        while (cur->right != nullptr) {
            cur = cur->right;
        }
        return cur;
    }

    void erase_min_(node *cur) {
        node *parent = cur;
        while (cur->left != nullptr) {
            parent = cur;
            --parent->size;
            cur = cur->left;
        }
        if (cur == parent) {
            root = cur->right;
        } else {
            parent->left = cur->right;
        }
        delete cur;
    }

    void erase_max_(node *cur) {
        node *parent = cur;
        while (cur->right != nullptr) {
            parent = cur;
            --parent->size;
            cur = cur->right;
        }
        if (cur == parent) {
            root = cur->left;
        } else {
            parent->right = cur->left;
        }
        delete cur;
    }

    node *unite_(node *l, node *r) {
        if (l == nullptr) {
            return r;
        }
        if (r == nullptr) {
            return l;
        }
        if (l->prior < r->prior) {
            std::swap(l, r);
        }
        node *tl, *tr;
        split(r, l->key, tl, tr);
        l->left = unite_(l->left, tl);
        l->right = unite_(l->right, tr);
        update_size(l);
        return l;
    }

    void copy_(node *&cur, node *other) {
        if (other != nullptr) {
            cur = new node(other);
            copy_(cur->left, other->left);
            copy_(cur->right, other->right);
        }
    }

    void array_(node *cur, T *&arr, int &index) {
        if (cur != nullptr) {
            array_(cur->left, arr, index);
            arr[++index] = cur->key;
            array_(cur->right, arr, index);
        }
    }

    void output_(node *cur) {
        if (cur != nullptr) {
            output_(cur->left);
            std::cout << cur->key << " ";
            output_(cur->right);
        }
    }

    void clear_(node *cur) {
        if (cur != nullptr) {
            clear_(cur->left);
            clear_(cur->right);
            delete cur;
        }
    }
};