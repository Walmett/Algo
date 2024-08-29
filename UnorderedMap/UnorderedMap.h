#pragma once

#include <vector>
#include <functional>

template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>,
        typename Alloc = std::allocator<std::pair<const Key, Value> > >
class UnorderedMap {

private:
    struct node;

    template<typename A>
    using common_allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<A>;
    using AllocTraits = std::allocator_traits<common_allocator<node> >;
    using StandardAllocTraits = std::allocator_traits<Alloc>;

    common_allocator<node> alloc;
    Alloc StandardAlloc = alloc;
    std::vector<std::pair<node *, node *>, common_allocator<std::pair<node *, node *> > > h;
    node *fake;
    node *head;
    size_t sz;
    float max_load = 4;
    Hash hash;
    Equal equal;

public:
    using NodeType = std::pair<const Key, Value>;

    UnorderedMap(const Alloc &alloc_ = Alloc())
            : alloc(alloc_), h(alloc), fake(AllocTraits::allocate(alloc, 1)), head(fake), sz(0) {
        fake->next = fake;
    }
    UnorderedMap(const UnorderedMap &other)
            : alloc(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.get_allocator())),
              h(other.h.size(), alloc), fake(AllocTraits::allocate(alloc, 1)), head(fake), sz(0), max_load(other.max_load) {
        fake->next = fake;
        for (auto it = other.begin(); it != other.end(); ++it) {
            node *new_node = construct_node(it.get_hash(), *it);
            emplace_by_node_(new_node);
        }
    }
    UnorderedMap(UnorderedMap &&other)
            : alloc(other.alloc), h(std::move(other.h), alloc), fake(other.fake), head(other.head),
              sz(other.sz), max_load(other.max_load) {
        other.fake = AllocTraits::allocate(alloc, 1);
        other.head = other.fake->next = other.fake;
        other.sz = 0;
    }
    ~UnorderedMap() {
        clear_();
        AllocTraits::deallocate(alloc, fake, 1);
    }
    UnorderedMap &operator=(const UnorderedMap &other) {
        clear_();
        if (std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value && alloc != other.get_allocator()) {
            alloc = other.alloc;
            StandardAlloc = other.StandardAlloc;
        }
        h = std::vector<std::pair<node *, node *>, common_allocator<std::pair<node *, node *> > >(alloc);
        for (auto it = other.begin(); it != other.end(); ++it) {
            node *new_node = construct_node(it.get_hash(), *it);
            emplace_by_node_(new_node);
        }
        return *this;
    }
    UnorderedMap &operator=(UnorderedMap &&other) {
        clear_();
        h = std::move(other.h);
        fake = other.fake;
        head = other.head;
        sz = other.sz;
        alloc = other.alloc;
        StandardAlloc = other.StandardAlloc;
        max_load = other.max_load;
        other.fake = AllocTraits::allocate(alloc, 1);
        other.head = other.fake->next = other.fake;
        other.sz = 0;
        return *this;
    }
    Alloc get_allocator() const {
        return alloc;
    }

    template<bool IsConst>
    struct common_iterator {
        friend common_iterator<false>;
        friend common_iterator<true>;

    private:
        using T_ = std::conditional_t<IsConst, const NodeType, NodeType>;
        node *ptr;

    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T_;
        using pointer = T_ *;
        using reference = T_ &;

        common_iterator() : ptr(nullptr) {}

        common_iterator(node *ptr_) : ptr(ptr_) {}
        ~common_iterator() {}
        common_iterator(const common_iterator &other) : ptr(other.ptr) {}
        common_iterator &operator=(const common_iterator &other) {
            ptr = other.ptr;
            return *this;
        }
        operator common_iterator<true>() const {
            return common_iterator<true>(ptr);
        }
        T_ &operator*() const {
            return ptr->data;
        }
        T_ *operator->() const {
            return &ptr->data;
        }
        common_iterator &operator++() {
            ptr = ptr->next;
            return *this;
        }
        common_iterator operator++(int) {
            common_iterator result = *this;
            ++*this;
            return result;
        }
        bool operator==(const common_iterator<true> &other) const {
            return ptr == other.ptr;
        }
        bool operator==(const common_iterator<false> &other) const {
            return ptr == other.ptr;
        }
        bool operator!=(const common_iterator<true> &other) const {
            return !(*this == other);
        }
        bool operator!=(const common_iterator<false> &other) const {
            return !(*this == other);
        }
        size_t get_hash() {
            return ptr->hsh;
        }
    };
    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    iterator begin() {
        return iterator(fake->next);
    }
    iterator end() {
        return iterator(fake);
    }
    const_iterator begin() const {
        return const_iterator(fake->next);
    }
    const_iterator end() const {
        return const_iterator(fake);
    }
    const_iterator cbegin() const {
        return const_iterator(fake->next);
    }
    const_iterator cend() const {
        return const_iterator(fake);
    }

    Value &operator[](const Key &key) {
        return get_(key);
    }
    Value &operator[](Key &&key) {
        return get_(std::move(key));
    }
    Value &at(const Key &key) {
        return get_throw_(key);
    }
    const Value &at(const Key &key) const {
        return get_throw_(key);
    }
    size_t size() const {
        return sz;
    }
    bool empty() const {
        return sz == 0;
    }
    size_t bucket_count() const {
        return h.size();
    }
    iterator find(const Key &key) {
        return find_(key);
    }
    iterator find(Key &&key) {
        return find_(std::move(key));
    }
    const_iterator find(const Key &key) const {
        return find_(key);
    }
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args &&... args) {
        return emplace_(std::forward<Args>(args)...);
    }
    template<typename InputIt>
    void insert(InputIt first, InputIt last) {
        while (first != last) {
            emplace_(*first);
            ++first;
        }
    }
    template<typename Node>
    std::pair<iterator, bool> insert(Node &&data) {
        return insert_(std::forward<Node>(data));
    }
    std::pair<iterator, bool> insert(NodeType &&data) {
        return insert_(std::move(data));
    }
    void erase(const Key &key) {
        erase_by_hash_(key, hash(key));
    }
    void erase(const_iterator it) {
        erase_by_hash_(it->first, it.get_hash());
    }
    void erase(const_iterator first, const_iterator last) {
        const_iterator prev = first;
        while (first != last) {
            ++first;
            erase_by_hash_(prev->first, prev.get_hash());
            prev = first;
        }
    }
    void clear() {
        clear_();
    }
    void rehash(size_t count) {
        rehash_(count);
    }
    void reserve(size_t count) {
        rehash_(count / max_load_factor() + 2);
    }
    float load_factor() const {
        return 1.0 * size() / bucket_count();
    }
    float max_load_factor() const {
        return max_load;
    }
    void max_load_factor(float ml) {
        max_load = ml;
    }

private:
    struct node {
        NodeType data;
        node *next;
        size_t hsh;

        node(NodeType *data_, size_t hsh_) : data(data_), hsh(hsh_) {}
    };

    template<typename ...Args>
    node *construct_node(size_t hsh, Args &&...args) {
        node *new_node = AllocTraits::allocate(alloc, 1);
        StandardAllocTraits::construct(StandardAlloc, &new_node->data, std::forward<Args>(args)...);
        new_node->hsh = hsh;
        return new_node;
    }

    void destroy_node(node *delete_node) {
        StandardAllocTraits::destroy(StandardAlloc, &delete_node->data);
        AllocTraits::deallocate(alloc, delete_node, 1);
    }

    void rehash_(size_t count) {
        std::vector<std::pair<node *, node *>, common_allocator<std::pair<node *, node *> > > tmp(count, alloc);
        node *cur = fake->next;
        while (cur != fake) {
            node *prev = cur;
            cur = cur->next;
            size_t index = prev->hsh % count;
            prev->next = tmp[index].first;
            tmp[index].first = prev;
        }
        cur = fake;
        for (size_t i = 0; i < count; ++i) {
            if (tmp[i].first != nullptr) {
                cur->next = tmp[i].first;
                tmp[i].second = cur;
                do {
                    cur = cur->next;
                } while (cur->next != nullptr);
            }
        }
        head = cur;
        cur->next = fake;
        h = std::move(tmp);
    }

    template<typename NodeKey>
    Value &get_(NodeKey &&key) {
        size_t hsh = hash(key);
        iterator it = find_by_hash_(key, hsh);
        if (it == end()) {
            node *new_node = construct_node(hsh, std::forward<NodeKey>(key), Value());
            return emplace_by_node_(new_node).first->second;
        }
        return it->second;
    }

    Value &get_throw_(const Key &key) const {
        iterator it = find_(key);
        if (it == end()) {
            throw std::out_of_range("the container does not have an element with the specified key");
        }
        return it->second;
    }

    iterator find_by_hash_(const Key &key, size_t hsh) const {
        if (!sz) {
            return iterator(fake);
        }
        size_t index = hsh % h.size();
        node *cur = h[index].first;
        if (cur == nullptr) {
            return iterator(fake);
        }
        do {
            if (equal(cur->data.first, key)) {
                return iterator(cur);
            }
            cur = cur->next;
        } while (cur != fake && cur->hsh % h.size() == index);
        return iterator(fake);
    }

    iterator find_(const Key &key) const {
        return find_by_hash_(key, hash(key));
    }

    float coefficient_() {
        return std::max(2.0f, 2 * max_load_factor());
    }

    void fix_table_() {
        if (bucket_count() < (size() / max_load_factor() + 1)) {
            rehash(coefficient_() * (size() / max_load_factor() + 1));
        }
    }

    std::pair<iterator, bool> emplace_by_node_(node *new_node) {
        fix_table_();
        size_t index = new_node->hsh % h.size();
        if (h[index].first == nullptr) {
            new_node->next = head->next;
            h[index].first = head->next = new_node;
            h[index].second = head;
            head = new_node;
            ++sz;
            return {iterator(new_node), true};
        }
        new_node->next = h[index].first;
        h[index].second->next = h[index].first = new_node;
        ++sz;
        return {iterator(new_node), true};
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace_(Args &&... args) {
        node *new_node = construct_node(0, std::forward<Args>(args)...);
        size_t hsh = hash(new_node->data.first);
        iterator it = find_by_hash_(new_node->data.first, new_node->hsh = hsh);
        if (it != end()) {
            destroy_node(new_node);
            return {it, false};
        }
        return emplace_by_node_(new_node);
    }

    template<typename Node>
    std::pair<iterator, bool> insert_(Node &&data) {
        size_t hsh = hash(data.first);
        iterator it = find_by_hash_(data.first, hsh);
        if (it != end()) {
            return {it, false};
        }
        node *new_node = construct_node(hsh, std::forward<Node>(data));
        return emplace_by_node_(new_node);
    }

    void erase_by_hash_(const Key &key, size_t hsh) {
        if (!sz) {
            return;
        }
        size_t index = hsh % h.size();
        std::pair<node *, node *> cur = h[index];
        if (cur.first == nullptr) {
            return;
        }
        do {
            if (equal(key, cur.first->data.first)) {
                if (cur.first == head) {
                    head = cur.second;
                }
                node *next = cur.first->next;
                size_t index_ = next->hsh % h.size();
                if (cur.second == fake || index != cur.second->hsh % h.size()) {
                    if (next == fake) {
                        h[index].first = h[index].second = nullptr;
                    } else if (index != index_) {
                        h[index_].second = cur.second;
                        h[index].first = h[index].second = nullptr;
                    } else {
                        h[index].first = next;
                    }
                }
                cur.second->next = next;
                destroy_node(cur.first);
                --sz;
                return;
            }
            cur.second = cur.first;
            cur.first = cur.first->next;
        } while (cur.first != fake && cur.first->hsh % h.size() == index);
    }

    void clear_() {
        for (node *cur = fake->next; cur != fake;) {
            node *tmp = cur;
            cur = cur->next;
            destroy_node(tmp);
        }
        head = fake->next = fake;
        sz = 0;
        h.assign(h.size(), {nullptr, nullptr});
    }
};