#pragma once

#include <iostream>
#include <vector>
#include <algorithm>


class FlowNetwork {
    friend class FlowNetworkBuilder;

public:
    FlowNetwork(size_t source, size_t sink, const std::vector<std::vector<int>> &capacity)
            : source_(source), sink_(sink), capacity_(capacity) {
        flow_.resize(capacity_.size(), std::vector<int>(capacity_.size()));
    }

    int64_t GetSourceCapacity() const {
        int64_t source_capacity = 0;
        for (int capacity: capacity_[source_]) {
            source_capacity += capacity;
        }
        return source_capacity;
    }

    int64_t GetCurrentFlow() const {
        int64_t current_flow = 0;
        for (int flow_value: flow_[sink_]) {
            current_flow -= flow_value;
        }
        return current_flow;
    }

    int64_t ComputeMaximumFlow() {
        PushRelabelMaximumFlow();
        return GetCurrentFlow();
    }

private:
    struct PushRelabelState {
        std::vector<int> excess;
        std::vector<int> height;
    };

    int ResidualFlow(size_t from, size_t to) const {
        return capacity_[from][to] - flow_[from][to];
    }

    void PushFlow(size_t from, size_t to, int add) {
        flow_[from][to] += add;
        flow_[to][from] -= add;
    }

    void InitializePushRelabel(PushRelabelState *state) {
        flow_.assign(capacity_.size(), std::vector<int>(capacity_.size()));
        state->excess.assign(capacity_.size(), 0);
        for (size_t i = 0; i < capacity_.size(); ++i) {
            int capacity = capacity_[source_][i];
            PushFlow(source_, i, capacity);
            state->excess[i] = capacity;
        }
        state->height.assign(capacity_.size(), 0);
        state->height[source_] = static_cast<int>(capacity_.size()) - 1;
    }

    std::vector<size_t> GetMaxHeightIndexes(const PushRelabelState &state) const {
        std::vector<size_t> max_height_indexes;
        int max_height = -1;
        for (size_t index = 0; index < capacity_.size(); ++index) {
            if (index != source_ && index != sink_ && state.excess[index] > 0) {
                if (state.height[index] > max_height) {
                    max_height = state.height[index];
                    max_height_indexes.clear();
                }
                if (state.height[index] == max_height) {
                    max_height_indexes.push_back(index);
                }
            }
        }
        return max_height_indexes;
    }

    bool TryPush(PushRelabelState *state, std::vector<size_t> &max_height_indexes) {
        bool is_pushed = false;
        const size_t from = max_height_indexes.back();
        for (size_t to = 0; to < capacity_.size() && state->excess[from] != 0; ++to) {
            if (ResidualFlow(from, to) > 0 && state->height[from] == state->height[to] + 1) {
                int add = std::min(ResidualFlow(from, to), state->excess[from]);
                PushFlow(from, to, add);
                state->excess[from] -= add;
                state->excess[to] += add;
                if (state->excess[from] == 0) {
                    max_height_indexes.pop_back();
                }
                is_pushed = true;
            }
        }
        return is_pushed;
    }

    void Lift(PushRelabelState *state, std::vector<size_t> &max_height_indexes) const {
        int max_height = state->height[max_height_indexes[0]];
        const size_t from = max_height_indexes.back();
        state->height[from] = max_height + 1;
        for (size_t to = 0; to < capacity_.size(); ++to) {
            if (ResidualFlow(from, to) > 0 && state->height[to] + 1 < state->height[from]) {
                state->height[from] = state->height[to] + 1;
            }
        }
        if (state->height[from] > max_height) {
            max_height_indexes.clear();
        }
    }

    void PushRelabelMaximumFlow() {
        PushRelabelState state;
        InitializePushRelabel(&state);
        while (true) {
            std::vector<size_t> max_height_indexes = GetMaxHeightIndexes(state);
            if (max_height_indexes.empty()) {
                break;
            }
            while (!max_height_indexes.empty()) {
                if (!TryPush(&state, max_height_indexes)) {
                    Lift(&state, max_height_indexes);
                }
            }
        }
    }

private:
    size_t source_;
    size_t sink_;
    std::vector<std::vector<int>> capacity_;
    std::vector<std::vector<int>> flow_;
};

class FlowNetworkBuilder {
public:
    FlowNetworkBuilder(size_t vertex_count)
            : flow_(0, vertex_count - 1,
                    std::vector<std::vector<int>>(vertex_count, std::vector<int>(vertex_count))) {

    }

    void SetSource(size_t source) {
        flow_.source_ = source;
    }

    void SetSink(size_t sink) {
        flow_.sink_ = sink;
    }

    void AddEdge(size_t from, size_t to, size_t capacity) {
        flow_.capacity_[from][to] = capacity;
    }

    FlowNetwork GetFlowNetwork() {
        return flow_;
    }

private:
    FlowNetwork flow_;
};