#pragma once

#include <iostream>
#include <vector>
#include <array>

class SuffixAutomaton {
public:
    SuffixAutomaton() : state(1, Node(-1, 0)) {}

    void Add(const std::string &str) {
        for (char c: str) {
            int letter = std::tolower(c) - 'a';
            PushLetter(letter);
        }
    }

    bool Check(const std::string &str) {
        int current = 0;
        for (char c: str) {
            int letter = std::tolower(c) - 'a';
            current = state[current].next[letter];
            if (current == -1) {
                return false;
            }
        }
        return true;
    }

private:
    struct Node {
        std::array<int, 26> next;
        int link;
        int length;

        Node(int link, int length) : link(link), length(length) {
            next.fill(-1);
        }
    };

    void PushLetter(int letter) {
        state.emplace_back(0, state[last].length + 1);
        int current = last;
        last = static_cast<int>(state.size()) - 1;
        while (current != -1) {
            if (state[current].next[letter] != -1) {
                int vertex = state[current].next[letter];
                if (state[vertex].length == state[current].length + 1) {
                    state[last].link = vertex;
                    return;
                }
                int clone = static_cast<int>(state.size());
                state.emplace_back(state[vertex].link, state[current].length + 1);
                state[clone].next = state[vertex].next;
                state[vertex].link = state[last].link = clone;
                while (current != -1 && state[current].next[letter] == vertex) {
                    state[current].next[letter] = clone;
                    current = state[current].link;
                }
                return;
            }
            state[current].next[letter] = last;
            current = state[current].link;
        }
    }

    int last{0};
    std::vector<Node> state;
};