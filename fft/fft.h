#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

class Polynomial {
public:
    Polynomial(int degree = -1) : polynomial(degree + 1) {}

    Polynomial(const Polynomial &other) : polynomial(other.polynomial) {}

    Polynomial(Polynomial &&other) : polynomial(std::move(other.polynomial)) {}

    auto &operator[](size_t index) {
        return polynomial[index];
    }

    auto operator[](size_t index) const {
        return polynomial[index];
    }

    Polynomial operator*(const Polynomial &poly) const {
        return Multiply(*this, poly);
    }

    void SetDegree(int new_degree) {
        polynomial.resize(new_degree + 1);
    }

    int GetDegree() {
        return int(polynomial.size()) - 1;
    }

private:
    static const int mod = 998244353;
    std::vector<int> polynomial;

    int64_t BinPow(int64_t a, int64_t power) const {
        int64_t result = 1;
        while (power) {
            if (power & 1) {
                result = (result * a) % mod;
            }
            a = (a * a) % mod;
            power >>= 1;
        }
        return result;
    }

    int64_t GetReversed(int64_t a) const {
        return BinPow(a, mod - 2);
    }

    std::vector<int> GetRev(int size) const {
        std::vector<int> rev(size);
        int log = 0;
        while (size > 1) {
            size >>= 1;
            ++log;
        }
        int oldest = -1;
        for (int mask = 1; mask < (1 << log); ++mask) {
            if (!(mask & (mask - 1))) {
                ++oldest;
            }
            rev[mask] = rev[mask ^ (1 << oldest)] | (1 << (log - oldest - 1));
        }
        return rev;
    }

    std::vector<int> GetRoots(int pow2, bool reverse) const {
        const int first_root = 31;
        const int reversed_root = 128805723;
        int64_t base_root = reverse ? reversed_root : first_root;
        std::vector<int> root(pow2 >> 1);
        for (; pow2 < (1 << 23); pow2 <<= 1) {
            base_root = (base_root * base_root) % mod;
        }
        root[0] = 1;
        for (int i = 1; i < static_cast<int>(root.size()); ++i) {
            root[i] = (root[i - 1] * base_root) % mod;
        }
        return root;
    }

    void Fft(std::vector<int> &cf, const std::vector<int> &rev, bool reverse = false) const {
        for (int i = 0; i < static_cast<int>(cf.size()); ++i) {
            if (i < rev[i]) {
                std::swap(cf[i], cf[rev[i]]);
            }
        }
        for (int pow = 2; pow <= static_cast<int>(cf.size()); pow <<= 1) {
            auto root = GetRoots(pow, reverse);
            for (int i = 0; i < static_cast<int>(cf.size()); i += pow) {
                for (int s = 0; s < (pow >> 1); ++s) {
                    int x = cf[i + s];
                    int y = (1ll * cf[i + s + (pow >> 1)] * root[s]) % mod;
                    cf[i + s] = x + y;
                    if (x + y >= mod) cf[i + s] -= mod;
                    cf[i + s + (pow >> 1)] = x - y;
                    if (x - y < 0) cf[i + s + (pow >> 1)] += mod;
                }
            }
        }
        if (reverse) {
            int64_t reversed_n = GetReversed(cf.size());
            for (int &c: cf) {
                c = (c * reversed_n) % mod;
            }
        }
    }

    Polynomial Multiply(const Polynomial &poly1, const Polynomial &poly2) const {
        std::vector<int> cf_first(poly1.polynomial);
        std::vector<int> cf_second(poly2.polynomial);
        int pow2 = 1;
        while (pow2 < static_cast<int>(std::max(cf_first.size(), cf_second.size()))) pow2 <<= 1;
        cf_first.resize(pow2 << 1);
        cf_second.resize(cf_first.size());
        for (int i = 0; i < static_cast<int>(cf_first.size()); ++i) {
            if (cf_first[i] < 0) {
                cf_first[i] += mod;
            }
            if (cf_second[i] < 0) {
                cf_second[i] += mod;
            }
        }
        std::vector<int> rev = GetRev(cf_first.size());
        Fft(cf_first, rev);
        Fft(cf_second, rev);
        for (int i = 0; i < static_cast<int>(cf_first.size()); ++i) {
            cf_first[i] = (1ll * cf_first[i] * cf_second[i]) % mod;
        }
        Fft(cf_first, rev, true);
        Polynomial result(int(cf_first.size()) - 1);
        for (int i = 0; i < static_cast<int>(cf_first.size()); ++i) {
            result[i] = cf_first[i];
        }
        for (int i = result.GetDegree(); i >= 0; --i) {
            if (result[i]) {
                result.SetDegree(i);
                return result;
            }
        }
        result.SetDegree(-1);
        return result;
    }
};