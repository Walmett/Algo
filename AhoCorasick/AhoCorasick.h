#pragma once

#include <iostream>
#include <vector>
#include <array>

class AhoCorasick {
public:
    AhoCorasick() : nodes_(1, Node(0, 0, 0, 0)) {
        nodes_[0].next_link.fill(0);
    }

    AhoCorasick(const AhoCorasick &other) = delete;

    void Push(const std::string &string) {
        final_links_updated_ = false;
        int cur = 0;
        for (char character: string) {
            if (nodes_[cur].next_link[character - kFirstIndex] <= 0) {
                CreateNode(cur, character - kFirstIndex);
            }
            cur = nodes_[cur].next_link[character - kFirstIndex];
        }
        ++nodes_[cur].count;
    }

    std::vector<std::vector<int>> BuildAutomaton();

private:
    static const int kAlphabetSize = 26;
    static const int kFirstIndex = 'a';
    static const int kNotFound = -1;

    struct Node {
        std::array<int, kAlphabetSize> next_link{};
        int link;
        int prev;
        int count;
        char last;
        int final_link = kNotFound;

        Node(int link, int prev, int count, char last) : link(link), prev(prev), count(count),
                                                         last(last) {}
    };

    std::vector<Node> nodes_;
    bool final_links_updated_ = false;

    void CreateNode(int v, char edge) {
        nodes_[v].next_link[edge] = nodes_.size();
        nodes_.emplace_back(v ? kNotFound : 0, v, 0, edge);
        nodes_.back().next_link.fill(int(kNotFound));
    }

    int GetLink(int vertex) {
        if (nodes_[vertex].link == kNotFound) {
            nodes_[vertex].link = GetNextLink(GetLink(nodes_[vertex].prev), nodes_[vertex].last);
        }
        return nodes_[vertex].link;
    }

    int GetNextLink(int vertex, char edge) {
        if (nodes_[vertex].next_link[edge] == kNotFound) {
            nodes_[vertex].next_link[edge] = GetNextLink(GetLink(vertex), edge);
        }
        return nodes_[vertex].next_link[edge];
    }

    int GetFinalLink(int vertex) {
        if (nodes_[vertex].final_link == kNotFound) {
            if (nodes_[vertex].count) {
                nodes_[vertex].final_link = vertex;
            } else {
                nodes_[vertex].final_link = GetFinalLink(GetLink(vertex));
            }
        }
        return nodes_[vertex].final_link;
    }

    void UpdateFinalLink() {
        nodes_[0].final_link = 0;
        for (size_t i = 1; i < nodes_.size(); ++i) {
            nodes_[i].final_link = kNotFound;
        }
        for (size_t i = 1; i < nodes_.size(); ++i) {
            GetFinalLink(i);
        }
        final_links_updated_ = true;
    }
};

std::vector<std::vector<int>> AhoCorasick::BuildAutomaton() {
    if (!final_links_updated_) {
        UpdateFinalLink();
    }

    std::vector<std::vector<int>> automaton(nodes_.size() + 1, std::vector<int>(26));

    for (size_t vertex = 0; vertex < nodes_.size(); ++vertex) {
        if (nodes_[vertex].final_link != 0) {
            continue;
        }
        for (int edge = 0; edge < 26; ++edge) {
            int to = GetNextLink(vertex, edge);
            if (nodes_[to].final_link == 0) {
                automaton[vertex + 1][edge] = to + 1;
            }
        }
    }

    return automaton;
}