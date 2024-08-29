#pragma once

#include <iostream>

template<typename T>
class AVL {
public:
    AVL() {
        root = nullptr;
    }
    ~AVL() {
        clear_(root);
    }
    AVL(const AVL &) = delete;
    AVL &operator=(const AVL &) = delete;

    void insert(const T &value) { root = insert_(root, value); }
    void erase(const T &value) { root = erase_(root, value); }
    bool find(const T &value) { return find_(root, value); }
    T *next(const T &value) { return next_(root, value); }
    T *prev(const T &value) { return prev_(root, value); }

private:
    struct node {
        T value;
        int8_t height;
        node *left;
        node *right;
        node(const T value_) : value(value_), height(1), left(nullptr), right(nullptr) {}
    };
    node *root;

    int8_t get_height(const node *cur) {
        if (cur == nullptr) {
            return 0;
        }
        return cur->height;
    }
    int8_t height_delta(const node *cur) {
        return get_height(cur->right) - get_height(cur->left);
    }
    void update_height(node *cur) {
        cur->height = std::max(get_height(cur->left), get_height(cur->right)) + 1;
    }
    node *rotate_left(node *cur) {
        node *tmp = cur->right;
        cur->right = tmp->left;
        tmp->left = cur;
        update_height(cur);
        update_height(tmp);
        return tmp;
    }
    node *rotate_right(node *cur) {
        node *tmp = cur->left;
        cur->left = tmp->right;
        tmp->right = cur;
        update_height(cur);
        update_height(tmp);
        return tmp;
    }
    node *balance(node *cur) {
        update_height(cur);
        if (height_delta(cur) == 2) {
            if (height_delta(cur->right) < 0) {
                cur->right = rotate_right(cur->right);
            }
            return rotate_left(cur);
        } else if (height_delta(cur) == -2) {
            if (height_delta(cur->left) > 0) {
                cur->left = rotate_left(cur->left);
            }
            return rotate_right(cur);
        }
        return cur;
    }
    bool find_(node *cur, const T &value) {
        while (cur != nullptr && cur->value != value) {
            if (value < cur->value) {
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        return cur != nullptr;
    }
    node *insert_(node *cur, const T &value) {
        if (cur == nullptr) {
            return new node(value);
        }
        if (value < cur->value) {
            cur->left = insert_(cur->left, value);
        } else {
            if (value == cur->value) {
                return cur;
            }
            cur->right = insert_(cur->right, value);
        }
        return balance(cur);
    }
    node *get_and_erase_max(node *cur, node *&t) {
        if (cur->right == nullptr) {
            t = cur;
            return cur->left;
        }
        cur->right = get_and_erase_max(cur->right, t);
        return balance(cur);
    }
    node *erase_(node *cur, const T &value) {
        if (cur == nullptr) {
            return nullptr;
        }
        if (value == cur->value) {
            node *l = cur->left;
            node *r = cur->right;
            delete cur;
            if (r == nullptr) {
                return l;
            }
            if (l == nullptr) {
                return r;
            }
            node *t;
            l = get_and_erase_max(l, t);
            t->left = l;
            t->right = r;
            return balance(t);
        }
        if (value < cur->value) {
            cur->left = erase_(cur->left, value);
        } else {
            cur->right = erase_(cur->right, value);
        }
        return balance(cur);
    }
    T *next_(node *cur, const T &value) {
        T *result = nullptr;
        while (cur != nullptr) {
            if (value < cur->value) {
                if (result == nullptr || cur->value < *result) {
                    result = &cur->value;
                }
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        return result;
    }
    T *prev_(node *cur, const T &value) {
        T *result = nullptr;
        while (cur != nullptr) {
            if (cur->value < value) {
                if (result == nullptr || cur->value > *result) {
                    result = &cur->value;
                }
                cur = cur->right;
            } else {
                cur = cur->left;
            }
        }
        return result;
    }
    void clear_(node *cur) {
        if (cur != nullptr) {
            clear_(cur->left);
            clear_(cur->right);
            delete cur;
        }
    }
};