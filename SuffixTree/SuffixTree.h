#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <map>

class SuffixTree {
public:
    void BuildTree(std::string str) {
        string_ = std::move(str);
        if (string_.back() != '$') {
            string_ += '$';
        }
        nodes_.clear();
        nodes_.emplace_back(-1, -1, 0);
        active_.vertex = 0;
        active_.edge = -1;
        active_.length = 0;
        reminder_ = 0;
        for (int i = 0; i < static_cast<int>(string_.size()); ++i) {
            PushLetter(i);
        }
    }

    void UpdateLink(int vertex, int link) {
        if (vertex != -1) {
            nodes_[vertex].link = link;
        }
    }

    void AddLeaf() {
        nodes_.emplace_back(active_.edge, string_.size(), 0);
        nodes_[active_.vertex].next[string_[active_.edge]] = static_cast<int>(nodes_.size()) - 1;
        UpdateLink(prev_split_vertex_, active_.vertex);
        prev_split_vertex_ = static_cast<int>(nodes_.size()) - 1;
        if (active_.vertex) {
            active_.vertex = nodes_[active_.vertex].link;
        } else {
            ++active_.edge;
            --active_.length;
        }
        --reminder_;
    }

    void Split(int split_vertex, int index) {
        nodes_.emplace_back(nodes_[split_vertex].left, nodes_[split_vertex].left + active_.length,
                            0);
        int mid_vertex = static_cast<int>(nodes_.size()) - 1;
        nodes_[active_.vertex].next[string_[active_.edge]] = mid_vertex;
        nodes_.emplace_back(index, string_.size(), 0);
        nodes_[mid_vertex].next[string_[index]] = static_cast<int>(nodes_.size()) - 1;
        nodes_[split_vertex].left += active_.length;
        nodes_[mid_vertex].next[string_[nodes_[split_vertex].left]] = split_vertex;
        UpdateLink(prev_split_vertex_, mid_vertex);
        prev_split_vertex_ = mid_vertex;
        if (active_.vertex) {
            active_.vertex = nodes_[active_.vertex].link;
        } else {
            ++active_.edge;
            --active_.length;
        }
        --reminder_;
    }

    void PushLetter(int index) {
        if (!reminder_++) {
            active_.edge = index;
            active_.length = std::max(active_.length, 0);
        }
        prev_split_vertex_ = -1;
        while (reminder_) {
            if (nodes_[active_.vertex].next.Has(string_[active_.edge])) {
                int split_vertex = nodes_[active_.vertex].next[string_[active_.edge]];
                int vertex_length =
                        std::min(index + 1, nodes_[split_vertex].right) - nodes_[split_vertex].left;
                if (vertex_length <= active_.length) {
                    active_.vertex = split_vertex;
                    active_.edge += vertex_length;
                    active_.length -= vertex_length;
                } else {
                    if (string_[index] == string_[nodes_[split_vertex].left + active_.length]) {
                        UpdateLink(prev_split_vertex_, active_.vertex);
                        ++active_.length;
                        return;
                    }
                    Split(split_vertex, index);
                }
            } else {
                AddLeaf();
            }
        }
    }

private:
    struct NodeNextArray {
        std::map<char, int> array;

        int &operator[](int index) {
            return array[static_cast<char>(index)];
        }

        bool Has(int index) {
            return array.find(static_cast<char>(index)) != array.end();
        }
    };

    struct Node {
        NodeNextArray next;
        int left;
        int right;
        int link;

        Node(int left, int right, int link) : left(left), right(right), link(link) {}
    };

    struct ActivePoint {
        int vertex;
        int edge;
        int length;

        ActivePoint() : vertex(0), edge(-1), length(0) {}
    };

    std::string string_;
    std::vector<Node> nodes_;
    ActivePoint active_;
    int reminder_ = 0;
    int prev_split_vertex_ = -1;
};