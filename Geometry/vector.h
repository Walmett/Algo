#pragma once

#include <array>
#include <cstddef>
#include <cmath>

class Vector;
double Length(const Vector& v);

class Vector {
public:
    Vector() = default;

    Vector(double x, double y, double z) : data_{x, y, z} {
    }

    Vector(const std::array<double, 3>& data) : data_(data) {
    }

    double& operator[](size_t ind) {
        return data_[ind];
    }

    double operator[](size_t ind) const {
        return data_[ind];
    }

    void Normalize() {
        double length = Length(*this);
        data_[0] /= length;
        data_[1] /= length;
        data_[2] /= length;
    }

private:
    std::array<double, 3> data_{};
};

double DotProduct(const Vector& a, const Vector& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector CrossProduct(const Vector& a, const Vector& b) {
    return {a[1] * b[2] - a[2] * b[1], -a[0] * b[2] + a[2] * b[0], a[0] * b[1] - a[1] * b[0]};
}

double Length(const Vector& v) {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

Vector operator+(const Vector& a, const Vector& b) {
    return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

Vector operator-(const Vector& a, const Vector& b) {
    return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

Vector operator*(const Vector& a, double coefficient) {
    return {coefficient * a[0], coefficient * a[1], coefficient * a[2]};
}

Vector operator*(double coefficient, const Vector& a) {
    return {coefficient * a[0], coefficient * a[1], coefficient * a[2]};
}