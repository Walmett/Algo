#pragma once

#include <iostream>
#include <vector>

void CountingSort(std::vector<size_t> &array, std::vector<size_t> &classes, size_t max_element) {
    std::vector<size_t> copy = array;
    std::vector<size_t> count(++max_element);
    for (size_t i = 0; i < array.size(); ++i) {
        ++count[classes[copy[i]]];
    }
    for (size_t i = 1; i < max_element; ++i) {
        count[i] += count[i - 1];
    }
    for (size_t i = array.size() - 1; i + 1 > 0; --i) {
        array[--count[classes[copy[i]]]] = copy[i];
    }
}

std::vector<size_t> GetCycleSort(const std::string &string) {
    std::vector<size_t> positions(string.size());
    std::vector<size_t> classes(string.size());
    for (size_t i = 0; i < string.size(); ++i) {
        positions[i] = i;
        classes[i] = string[i] - 'a';
    }
    CountingSort(positions, classes, 26);
    size_t current_class = classes[positions[0]] = 0;
    for (size_t i = 1; i < positions.size(); ++i) {
        if (string[positions[i]] != string[positions[i - 1]]) {
            ++current_class;
        }
        classes[positions[i]] = current_class;
    }

    for (size_t pow = 1; pow < positions.size(); pow <<= 1) {
        for (size_t i = 0; i < positions.size(); ++i) {
            if (positions[i] < pow) {
                positions[i] += positions.size();
            }
            positions[i] -= pow;
        }
        CountingSort(positions, classes, current_class + 1);
        std::vector<size_t> previous_classes = classes;
        current_class = classes[positions[0]] = 0;
        for (size_t i = 1; i < positions.size(); ++i) {
            size_t first_position = positions[i] + pow;
            size_t second_position = positions[i - 1] + pow;
            if (first_position >= positions.size()) {
                first_position -= positions.size();
            }
            if (second_position >= positions.size()) {
                second_position -= positions.size();
            }
            if (previous_classes[positions[i]] != previous_classes[positions[i - 1]] ||
                previous_classes[first_position] != previous_classes[second_position]) {
                ++current_class;
            }
            classes[positions[i]] = current_class;
        }
    }
    return positions;
}