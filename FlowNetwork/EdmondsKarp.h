#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

class Flow {
public:
    Flow(size_t source, size_t drain, const std::vector<std::vector<int>> &cost)
            : source_(source), drain_(drain), cost_(cost) {
        flow_.resize(cost_.size(), std::vector<int>(cost_.size()));
    }

    std::vector<std::vector<int>> GetMaxFlow() {
        const int INF = 1'000'000'000;
        if (source_ != 0 || drain_ != cost_.size() - 1) {
            return flow_;
        }
        while (true) {
            std::vector<int> from(cost_.size(), -1);
            std::vector<int> queue(cost_.size());
            int head = 0, tail = 0;
            from[0] = 0;
            ++tail;
            while (head < tail) {
                int current = queue[head++];
                for (size_t vertex = 0; vertex < cost_.size(); ++vertex) {
                    if (from[vertex] == -1 && cost_[current][vertex] - flow_[current][vertex] > 0) {
                        queue[tail++] = vertex;
                        from[vertex] = current;
                    }
                }
            }
            if (from[cost_.size() - 1] == -1) {
                break;
            }
            int coefficient = INF;
            size_t current = cost_.size() - 1;
            while (current != 0) {
                size_t previous = from[current];
                coefficient = std::min(coefficient,
                                       cost_[previous][current] - flow_[previous][current]);
                current = previous;
            }
            current = cost_.size() - 1;
            while (current != 0) {
                size_t previous = from[current];
                flow_[previous][current] += coefficient;
                flow_[current][previous] -= coefficient;
                current = previous;
            }
        }
        return flow_;
    }

    std::vector<std::vector<int>> GetCost() const {
        return cost_;
    }

private:
    size_t source_;
    size_t drain_;
    std::vector<std::vector<int>> cost_;
    std::vector<std::vector<int>> flow_;
};