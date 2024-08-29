#pragma once

#include <iostream>
#include <cassert>

template<typename T>
class Splay {
public:
    Splay() {
        root = nullptr;
    }
    ~Splay() {
        clear_(root);
    }
    Splay(const Splay &) = delete;
    Splay &operator=(const Splay &) = delete;

    void insert(const T value) { insert_(value); }
    int64_t sum(const int32_t l, const int32_t r) { return sum_(l, r); }
    bool find(const T value) {
        find_(root, value);
        return root->value == value;
    }

private:
    struct node {
        T value;
        int64_t sum;
        node *parent;
        node *left;
        node *right;
        node(const T value_) : value(value_), sum(value_), parent(nullptr), left(nullptr), right(nullptr) {}
    };
    node *root;

    void set_parent(node *cur, node *parent) {
        if (cur != nullptr) {
            cur->parent = parent;
        }
    }
    void sum_update(node *cur) {
        if (cur != nullptr) {
            cur->sum = cur->value;
            if (cur->left != nullptr) {
                cur->sum += cur->left->sum;
            }
            if (cur->right != nullptr) {
                cur->sum += cur->right->sum;
            }
        }
    }
    void rotate(node *cur) {
        node *parent = cur->parent;
        node *p_parent = parent->parent;
        if (p_parent != nullptr) {
            if (p_parent->left == parent) {
                p_parent->left = cur;
            } else {
                p_parent->right = cur;
            }
        }
        if (parent->left == cur) {
            parent->left = cur->right;
            cur->right = parent;
            parent->parent = cur;
            set_parent(parent->left, parent);
        } else {
            parent->right = cur->left;
            cur->left = parent;
            parent->parent = cur;
            set_parent(parent->right, parent);
        }
        cur->parent = p_parent;
        sum_update(parent);
    }
    node *splay(node *cur) {
        if (cur->parent == nullptr) {
            return cur;
        }
        node *parent = cur->parent;
        node *p_parent = parent->parent;
        if (p_parent == nullptr) {
            rotate(cur);
            return cur;
        }
        if ((p_parent->left == parent) ^ (parent->left == cur)) {
            rotate(cur);
            rotate(cur);
        } else {
            rotate(parent);
            rotate(cur);
        }
        return splay(cur);
    }
    node *find_(node *cur, const T value) {
        if (cur == nullptr) {
            return nullptr;
        }
        for (;;) {
            if (value < cur->value) {
                if (cur->left == nullptr) {
                    return splay(cur);
                }
                cur = cur->left;
            } else {
                if (cur->right == nullptr || value == cur->value) {
                    return splay(cur);
                }
                cur = cur->right;
            }
        }
    }
    void split(node *cur, const T value, node *&left, node *&right) {
        if (cur == nullptr) {
            left = right = nullptr;
            return;
        }
        cur = find_(cur, value);
        if (cur->value < value) {
            right = cur->right;
            cur->right = nullptr;
            set_parent(right, nullptr);
            left = cur;
        } else {
            left = cur->left;
            cur->left = nullptr;
            set_parent(left, nullptr);
            right = cur;
        }
        sum_update(left);
        sum_update(right);
    }
    node *merge(node *left, node *right) {
        if (left == nullptr) {
            return right;
        }
        if (right == nullptr) {
            return left;
        }
        left = find_(left, right->value);
        left->right = right;
        right->parent = left;
        return left;
    }
    void insert_(const T value) {
        if (root == nullptr) {
            root = new node(value);
            return;
        }
        node *cur = root;
        while (value != cur->value) {
            if (value < cur->value) {
                if (cur->left == nullptr) {
                    cur->left = new node(value);
                    cur->left->parent = cur;
                    root = splay(cur->left);
                    return;
                }
                cur = cur->left;
            } else {
                if (cur->right == nullptr) {
                    cur->right = new node(value);
                    cur->right->parent = cur;
                    root = splay(cur->right);
                    return;
                }
                cur = cur->right;
            }
        }
        root = splay(cur);
    }
    int64_t sum_(const int32_t l, const int32_t r) {
        assert(l <= r);
        node *left, *right;
        split(root, l, left, root);
        split(root, r + 1, root, right);
        int64_t ans = 0;
        if (root != nullptr) {
            ans = root->sum;
        }
        root = merge(left, root);
        root = merge(root, right);
        return ans;
    }
    void clear_(node *cur) {
        if (cur != nullptr) {
            clear_(cur->left);
            clear_(cur->right);
            delete cur;
        }
    }
};