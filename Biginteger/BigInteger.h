#pragma once

#include <iostream>
#include <vector>
#include <complex>

class BigInteger {
public:
    BigInteger() {
        digits.push_back(0);
    }
    ~BigInteger() = default;
    BigInteger(const BigInteger &other) {
        digits = other.digits;
        minus = other.minus;
    }
    BigInteger(int x) {
        if (x < 0) {
            x = -x;
            minus = true;
        }
        while (x) {
            digits.push_back(x % BASE);
            x /= BASE;
        }
        if (digits.empty()) {
            digits.push_back(0);
        }
    }
    explicit BigInteger(long long x) {
        if (x < 0) {
            x = -x;
            minus = true;
        }
        while (x) {
            digits.push_back(x % BASE);
            x /= BASE;
        }
        if (digits.empty()) {
            digits.push_back(0);
        }
    }
    explicit BigInteger(unsigned long long x) {
        while (x) {
            digits.push_back(x % BASE);
            x /= BASE;
        }
        if (digits.empty()) {
            digits.push_back(0);
        }
    }
    BigInteger(const std::string &s) {
        buildBigInteger(s);
    }
    BigInteger &operator=(const BigInteger &other) {
        digits = other.digits;
        minus = other.minus;
        return *this;
    }
    BigInteger &operator=(int x) {
        digits.clear();
        if (x < 0) {
            x = -x;
            minus = true;
        } else {
            minus = false;
        }
        while (x) {
            digits.push_back(x % BASE);
            x /= BASE;
        }
        if (digits.empty()) {
            digits.push_back(0);
        }
        return *this;
    }
    BigInteger &operator=(long long x) {
        digits.clear();
        if (x < 0) {
            x = -x;
            minus = true;
        } else {
            minus = false;
        }
        while (x) {
            digits.push_back(x % BASE);
            x /= BASE;
        }
        if (digits.empty()) {
            digits.push_back(0);
        }
        return *this;
    }
    bool operator==(const BigInteger &other) const {
        if (minus != other.minus) {
            return false;
        }
        return digits == other.digits;
    }
    bool operator!=(const BigInteger &other) const {
        return !(*this == other);
    }
    bool operator<(const BigInteger &other) const {
        if (minus != other.minus) {
            return minus;
        }
        if (digits.size() != other.digits.size()) {
            return (digits.size() < other.digits.size()) ^ minus;
        }
        for (size_t i = digits.size() - 1; i + 1; --i) {
            if (digits[i] != other.digits[i]) {
                return (digits[i] < other.digits[i]) ^ minus;
            }
        }
        return false;
    }
    bool operator>=(const BigInteger &other) const {
        return !(*this < other);
    }
    bool operator>(const BigInteger &other) const {
        return other < *this;
    }
    bool operator<=(const BigInteger &other) const {
        return !(*this > other);
    }
    bool operator==(int x) const {
        if (digits.size() > 2) {
            return false;
        }
        return static_cast<long long>(*this) == x;
    }
    bool operator!=(int x) const {
        return !(*this == x);
    }
    bool operator<(int x) const {
        if (digits.size() > 2) {
            return minus;
        }
        return static_cast<long long>(*this) < x;
    }
    bool operator>=(int x) const {
        return !(*this < x);
    }
    bool operator>(int x) const {
        if (digits.size() > 2) {
            return !minus;
        }
        return x < static_cast<long long>(*this);
    }
    bool operator<=(int x) const {
        return !(*this > x);
    }
    size_t length() {
        size_t result = (digits.size() - 1) * 9;
        int d = digits.back();
        while (d) {
            d /= 10;
            ++result;
        }
        return result;
    }
    operator bool() const {
        return digits.size() != 1 || digits.front();
    }
    operator long long() const {
        if (digits.size() > 2) {
            if (minus) {
                return -9223372036854775807ll - 1;
            }
            return 9223372036854775807ll;
        }
        long long result = 0;
        if (digits.size() == 2) {
            result = 1ll * BASE * digits[1];
        }
        result += digits.front();
        if (minus) {
            return -result;
        }
        return result;
    }
    operator int() const {
        long long tmp = *this;
        if (tmp > 2147483647) {
            return 2147483647;
        }
        if (tmp < -2147483647 - 1) {
            return -2147483647 - 1;
        }
        return tmp;
    }
    BigInteger &operator+=(const BigInteger &other) {
        if (minus == other.minus) {
            digits.front() += other.digits.front();
            for (size_t i = 1; i < other.digits.size() || digits[i - 1] >= BASE; ++i) {
                if (i < other.digits.size()) {
                    if (i == digits.size()) {
                        digits.push_back(other.digits[i]);
                    } else {
                        digits[i] += other.digits[i];
                    }
                }
                if (digits[i - 1] >= BASE) {
                    digits[i - 1] -= BASE;
                    if (i == digits.size()) {
                        digits.push_back(1);
                    } else {
                        ++digits[i];
                    }
                }
            }
        } else {
            minus = !minus;
            *this -= other;
            minus = !minus;
        }
        fixBigInteger(*this);
        return *this;
    }
    BigInteger &operator-=(const BigInteger &other) {
        if (minus == other.minus) {
            if ((!minus && *this < other) || (minus && *this > other)) {
                BigInteger tmp = other;
                *this = tmp -= *this;
                minus = !minus;
            } else {
                digits.front() -= other.digits.front();
                for (size_t i = 1; i < other.digits.size() || digits[i - 1] < 0; ++i) {
                    if (i < other.digits.size()) {
                        if (i == digits.size()) {
                            digits.push_back(-other.digits[i]);
                        } else {
                            digits[i] -= other.digits[i];
                        }
                    }
                    if (digits[i - 1] < 0) {
                        digits[i - 1] += BASE;
                        if (i == digits.size()) {
                            digits.push_back(-1);
                        } else {
                            --digits[i];
                        }
                    }
                }
                fixBigInteger(*this);
            }
        } else {
            minus = !minus;
            *this += other;
            minus = !minus;
        }
        fixBigInteger(*this);
        return *this;
    }
    BigInteger &operator+=(int x) {
        return *this += BigInteger(x);
    }
    BigInteger &operator-=(int x) {
        return *this -= BigInteger(x);
    }
    BigInteger operator+(const BigInteger &other) const {
        return BigInteger(*this) += other;
    }
    BigInteger operator-(const BigInteger &other) const {
        return BigInteger(*this) -= other;
    }
    BigInteger operator-() const {
        BigInteger result(*this);
        result.minus = !result.minus;
        fixBigInteger(result);
        return result;
    }
    BigInteger &operator++() {
        return *this += BigInteger(1);
    }
    BigInteger operator++(int) {
        BigInteger tmp(*this);
        ++(*this);
        return tmp;
    }
    BigInteger &operator--() {
        return *this -= BigInteger(1);
    }
    BigInteger operator--(int) {
        BigInteger tmp(*this);
        --(*this);
        return tmp;
    }
    BigInteger operator+(int x) const {
        return BigInteger(*this) += x;
    }
    BigInteger operator-(int x) const {
        return BigInteger(*this) -= x;
    }
    BigInteger operator*(const BigInteger &other) const {
        if ((!other.minus && other < 2147483647) || (other.minus && other > -2147483647 - 1)) {
            return *this * static_cast<int>(other);
        }
        if ((!minus && *this < 2147483647) || (minus && *this > -2147483647 - 1)) {
            return other * static_cast<int>(*this);
        }
        return mlt(*this, other);
    }
    BigInteger &operator*=(const BigInteger &other) {
        return *this = *this * other;
    }
    BigInteger operator*=(int x) {
        if (x < 0) {
            x = -x;
            minus = !minus;
        }
        for (size_t i = 0, add = 0; i < digits.size() || add; ++i) {
            if (i == digits.size()) {
                digits.push_back(0);
            }
            long long cur = 1ll * digits[i] * x + add;
            digits[i] = cur % BASE;
            add = cur / BASE;
        }
        fixBigInteger(*this);
        return *this;
    }
    BigInteger operator*(int x) const {
        return BigInteger(*this) *= x;
    }
    BigInteger operator/(const BigInteger &other) const {
        if ((!other.minus && other <= 2147483647) || (other.minus && other >= -2147483647 - 1)) {
            return *this / static_cast<int>(other);
        }
        if (!other) {
            return BigInteger();
        }
        std::string result;
        if (minus ^ other.minus) {
            result += '-';
        }
        BigInteger tmp;
        for (size_t i = digits.size() - 1; i + 1; --i) {
            int d = digits[i];
            int pow10 = BASE / 10;
            while (pow10) {
                tmp *= 10;
                tmp += d / pow10;
                d %= pow10;
                pow10 /= 10;
                char digit = '0';
                tmp.minus = other.minus;
                while ((!tmp.minus && other <= tmp) || (tmp.minus && tmp <= other)) {
                    int d1 = tmp.first2Digits();
                    int d2 = other.first2Digits();
                    int t = d1 / d2 - (((d2 * 10 + 9) * (d1 / d2) < d1 * 10 - 9) ? 0 : 1);
                    if (t > 3) {
                        tmp -= other * t;
                        digit += t;
                    } else {
                        tmp -= other;
                        ++digit;
                    }
                    tmp.minus = other.minus;
                }
                if (!result.empty() || digit != '0') {
                    result += digit;
                }
            }
        }
        return BigInteger(result);
    }
    BigInteger &operator/=(const BigInteger &other) {
        return *this = *this / other;
    }
    BigInteger &operator/=(int x) {
        if (!x) {
            return *this;
        }
        if (x < 0) {
            minus = !minus;
            x = -x;
        }
        for (size_t i = digits.size() - 1, add = 0; i + 1; --i) {
            long long cur = digits[i] + 1ll * add * BASE;
            digits[i] = cur / x;
            add = cur % x;
        }
        fixBigInteger(*this);
        return *this;
    }
    BigInteger operator/(int x) const {
        return BigInteger(*this) /= x;
    }
    BigInteger &operator%=(const BigInteger &other) {
        return *this -= ((*this / other) * other);
    }
    BigInteger operator%(const BigInteger &other) const {
        return BigInteger(*this) %= other;
    }
    BigInteger &operator%=(int x) {
        return *this -= ((*this / x) * x);
    }
    BigInteger operator%(int x) const {
        return BigInteger(*this) %= x;
    }
    friend std::istream &operator>>(std::istream &in, BigInteger &x) {
        std::string s;
        in >> s;
        x.buildBigInteger(s);
        return in;
    }
    friend std::ostream &operator<<(std::ostream &out, const BigInteger &x) {
        if (x.minus) {
            out << '-';
        }
        out << x.digits.back();
        for (size_t i = x.digits.size() - 2; i + 1; --i) {
            int pow10 = BASE / 10;
            while (x.digits[i] < pow10) {
                pow10 /= 10;
                out << '0';
            }
            if (x.digits[i]) {
                out << x.digits[i];
            }
        }
        return out;
    }
    std::string toString() const {
        return toString_();
    }
    void setPositive() {
        minus = false;
    }
    void flipSign() {
        minus = !minus;
    }
    bool isMinus() const {
        return minus;
    }
    bool isEven() const {
        return !(digits.front() & 1);
    }

private:
    static const int BASE = 1000000000;
    std::vector<int> digits;
    bool minus = false;

    void fixBigInteger(BigInteger &x) const {
        if (x.digits.empty()) {
            x.digits.push_back(0);
        } else {
            while (!x.digits.back() && x.digits.size() > 1) {
                x.digits.pop_back();
            }
        }
        if (x.digits.size() == 1 && !x.digits.front()) {
            x.minus = false;
        }
    }
    void buildBigInteger(const std::string &s) {
        digits.clear();
        if (s.empty()) {
            minus = false;
            digits.push_back(0);
            return;
        }
        if (s.front() == '-') {
            minus = true;
        } else {
            minus = false;
        }
        digits.resize((s.size() - 1 - minus) / 9 + 1);
        int pow = 1;
        size_t p = 0;
        for (size_t i = s.size() - 1; i + 1 > minus; --i) {
            digits[p] += (s[i] - '0') * pow;
            pow *= 10;
            if (pow == BASE) {
                pow = 1;
                ++p;
            }
        }
        fixBigInteger(*this);
    }
    std::string toString_() const {
        std::string result;
        if (minus) {
            result += '-';
        }
        std::string tmp;
        int x = digits.back();
        while (x) {
            tmp += '0' + x % 10;
            x /= 10;
        }
        if (tmp.empty()) {
            tmp += '0';
        }
        for (size_t j = tmp.size(); j; --j) {
            result += tmp[j - 1];
        }
        for (size_t i = digits.size() - 2; i + 1; --i) {
            int pow10 = BASE / 10;
            while (digits[i] < pow10) {
                pow10 /= 10;
                result += '0';
            }
            if (digits[i]) {
                tmp.clear();
                x = digits[i];
                while (x) {
                    tmp += '0' + x % 10;
                    x /= 10;
                }
                for (size_t j = tmp.size(); j; --j) {
                    result += tmp[j - 1];
                }
            }
        }
        return result;
    }
    void fft(std::vector<std::complex<double>> &a, bool invert) const {
        int n = a.size();
        for (int i = 1, j = 0; i < n; ++i) {
            int bit = n >> 1;
            for (; j >= bit; bit >>= 1) {
                j -= bit;
            }
            j += bit;
            if (i < j) {
                swap(a[i], a[j]);
            }
        }
        for (int len = 2; len <= n; len <<= 1) {
            double ang = 2 * M_PI / len * (invert ? -1 : 1);
            std::complex<double> w_length(cos(ang), sin(ang));
            for (int i = 0; i < n; i += len) {
                std::complex<double> w(1);
                for (int j = 0; j < len >> 1; ++j) {
                    std::complex<double> u = a[i + j], v = a[i + j + (len >> 1)] * w;
                    a[i + j] = u + v;
                    a[i + j + (len >> 1)] = u - v;
                    w *= w_length;
                }
            }
        }
        if (invert) {
            for (int i = 0; i < n; ++i) {
                a[i] /= n;
            }
        }
    }
    void multiply(const std::vector<int> &a, const std::vector<int> &b, std::vector<int> &res) const {
        std::vector<std::complex<double>> fa(a.begin(), a.end()), fb(b.begin(), b.end());
        size_t n = 1;
        while (n < std::max(a.size(), b.size())) {
            n <<= 1;
        }
        n <<= 1;
        fa.resize(n);
        fb.resize(n);
        fft(fa, false);
        fft(fb, false);
        for (size_t i = 0; i < n; ++i) {
            fa[i] *= fb[i];
        }
        fft(fa, true);
        res.resize(n);
        for (size_t i = 0; i < n; ++i) {
            res[i] = int(fa[i].real() + 0.5);
        }
        int add = 0;
        for (size_t i = 0; i < n; ++i) {
            res[i] += add;
            add = res[i] / 10;
            res[i] %= 10;
        }
    }
    BigInteger mlt(const BigInteger &a, const BigInteger &b) const {
        std::string ta = a.toString_();
        std::string tb = b.toString_();
        std::vector<int> va(ta.size() - (ta[0] == '-'));
        std::vector<int> vb(tb.size() - (tb[0] == '-'));
        for (size_t i = 0; i < va.size(); ++i) {
            va[i] = ta[ta.size() - i - 1] - '0';
        }
        for (size_t i = 0; i < vb.size(); ++i) {
            vb[i] = tb[tb.size() - i - 1] - '0';
        }
        std::vector<int> result;
        multiply(va, vb, result);
        ta.clear();
        if (a.minus ^ b.minus) {
            ta += '-';
        }
        for (size_t i = result.size() - 1; i + 1; --i) {
            ta += result[i] + '0';
        }
        return BigInteger(ta);
    }
    int first2Digits() const {
        long long d = digits.back();
        if (digits.size() > 1) {
            d *= BASE;
            d += digits[digits.size() - 2];
        }
        while (d > 99) {
            d /= 10;
        }
        return d;
    }
};

bool operator==(int x, const BigInteger &y) {
    return y == x;
}
bool operator!=(int x, const BigInteger &y) {
    return y != x;
}
bool operator<(int x, const BigInteger &y) {
    return y > x;
}
bool operator>(int x, const BigInteger &y) {
    return y < x;
}
bool operator<=(int x, const BigInteger &y) {
    return y >= x;
}
bool operator>=(int x, const BigInteger &y) {
    return y <= x;
}
BigInteger operator+(int x, const BigInteger &y) {
    return BigInteger(y) += x;
}
BigInteger operator-(int x, const BigInteger &y) {
    return BigInteger(x) -= y;
}
BigInteger operator*(int x, const BigInteger &y) {
    return BigInteger(y) *= x;
}
BigInteger operator/(int x, const BigInteger &y) {
    return BigInteger(x) /= y;
}
BigInteger operator%(int x, const BigInteger &y) {
    return BigInteger(x) %= y;
}
BigInteger operator "" _bi(unsigned long long value) {
    return BigInteger(value);
}

class Rational {
public:
    Rational() {
        p = BigInteger(0);
        q = BigInteger(1);
    }
    ~Rational() = default;
    Rational(const BigInteger &other) {
        p = other;
        q = BigInteger(1);
    }
    Rational(const Rational &other) {
        p = other.p;
        q = other.q;
    }
    Rational(int x) {
        p = BigInteger(x);
        q = BigInteger(1);
    }
    explicit Rational(unsigned long long x) {
        p = BigInteger(x);
        q = BigInteger(1);
    }
    Rational &operator=(const Rational &other) {
        p = other.p;
        q = other.q;
        return *this;
    }
    Rational &operator+=(const Rational &other) {
        p *= other.q;
        p += other.p * q;
        q *= other.q;
        BigInteger tmp = gcd(p, q);
        p /= tmp;
        q /= tmp;
        return *this;
    }
    Rational &operator-=(const Rational &other) {
        p *= other.q;
        p -= other.p * q;
        q *= other.q;
        BigInteger tmp = gcd(p, q);
        p /= tmp;
        q /= tmp;
        return *this;
    }
    Rational &operator*=(const Rational &other) {
        p *= other.p;
        q *= other.q;
        BigInteger tmp = gcd(p, q);
        p /= tmp;
        q /= tmp;
        return *this;
    }
    Rational &operator/=(const Rational &other) {
        p *= other.q;
        q *= other.p;
        if (q.isMinus()) {
            q.setPositive();
            p.flipSign();
        }
        BigInteger tmp = gcd(p, q);
        p /= tmp;
        q /= tmp;
        return *this;
    }
    Rational &operator+=(const BigInteger &other) {
        return *this += Rational(other);
    }
    Rational &operator-=(const BigInteger &other) {
        return *this -= Rational(other);
    }
    Rational &operator*=(const BigInteger &other) {
        return *this *= Rational(other);
    }
    Rational &operator/=(const BigInteger &other) {
        return *this /= Rational(other);
    }
    Rational &operator+=(int other) {
        return *this += Rational(other);
    }
    Rational &operator-=(int other) {
        return *this -= Rational(other);
    }
    Rational &operator*=(int other) {
        return *this *= Rational(other);
    }
    Rational &operator/=(int other) {
        return *this /= Rational(other);
    }
    Rational operator+(const Rational &other) const {
        return Rational(*this) += other;
    }
    Rational operator-(const Rational &other) const {
        return Rational(*this) -= other;
    }
    Rational operator*(const Rational &other) const {
        return Rational(*this) *= other;
    }
    Rational operator/(const Rational &other) const {
        return Rational(*this) /= other;
    }
    Rational operator+(const BigInteger &other) const {
        return Rational(*this) += Rational(other);
    }
    Rational operator-(const BigInteger &other) const {
        return Rational(*this) -= Rational(other);
    }
    Rational operator*(const BigInteger &other) const {
        return Rational(*this) *= Rational(other);
    }
    Rational operator/(const BigInteger &other) const {
        return Rational(*this) /= Rational(other);
    }
    Rational operator+(int other) const {
        return Rational(*this) += Rational(other);
    }
    Rational operator-(int other) const {
        return Rational(*this) -= Rational(other);
    }
    Rational operator*(int other) const {
        return Rational(*this) *= Rational(other);
    }
    Rational operator/(int other) const {
        return Rational(*this) /= Rational(other);
    }
    Rational operator-() const {
        Rational result(*this);
        if (result) {
            result.p.flipSign();
        }
        return result;
    }
    bool operator==(const Rational &other) const {
        return p == other.p && q == other.q;
    }
    bool operator!=(const Rational &other) const {
        return p != other.p || q != other.q;
    }
    bool operator<(const Rational &other) const {
        return p * other.q < other.p * q;
    }
    bool operator>(const Rational &other) const {
        return p * other.q > other.p * q;
    }
    bool operator<=(const Rational &other) const {
        return p * other.q <= other.p * q;
    }
    bool operator>=(const Rational &other) const {
        return p * other.q >= other.p * q;
    }
    bool operator==(int x) const {
        return *this == Rational(x);
    }
    bool operator!=(int x) const {
        return *this != Rational(x);
    }
    bool operator<(int x) const {
        return *this < Rational(x);
    }
    bool operator>(int x) const {
        return *this > Rational(x);
    }
    bool operator<=(int x) const {
        return *this <= Rational(x);
    }
    bool operator>=(int x) const {
        return *this >= Rational(x);
    }
    operator bool() const {
        return p;
    }
    operator double() const {
        return std::stod(asDecimal(32));
    }
    std::string toString() const {
        return toString_();
    }
    std::string asDecimal(size_t precision = 0) const {
        return asDecimal_(precision);
    }
    friend std::istream &operator>>(std::istream &in, Rational &x) {
        std::string s;
        in >> s;
        if (s.empty()) {
            x = Rational();
            return in;
        }
        int ps = -1, pd = -1;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '/') {
                ps = i;
                break;
            } else if (s[i] == '.') {
                pd = i;
                break;
            }
        }
        if (~ps && ~pd) {
            if (~ps) {
                x.p = BigInteger(s.substr(0, ps));
                x.q = BigInteger(s.substr(ps + 1, s.size() - ps - 1));
            } else {
                size_t cnt = s.size() - pd - 1;
                s.erase(pd, 1);
                x.p = BigInteger(s);
                s.clear();
                s += '1';
                for (size_t i = 0; i < cnt; ++i) {
                    s += '0';
                }
                x.q = BigInteger(s);
            }
        } else {
            x.p = BigInteger(s);
            x.q = BigInteger(1);
        }
        return in;
    }
    friend std::ostream &operator<<(std::ostream &out, const Rational &x) {
        out << x.asDecimal(out.precision());
        return out;
    }

private:
    BigInteger p;
    BigInteger q;

    BigInteger gcd(BigInteger a, BigInteger b) const {
        a.setPositive();
        b.setPositive();
        int d = 0;
        if (!a || !b) {
            return a + b;
        }
        while (a.isEven() && b.isEven()) {
            ++d;
            a /= 2;
            b /= 2;
        }
        while (a && b) {
            if (!b.isEven()) {
                while (a.isEven()) {
                    a /= 2;
                }
            } else {
                while (b.isEven()) {
                    b /= 2;
                }
            }
            if (a >= b) {
                a -= b;
                a /= 2;
            } else {
                b -= a;
                b /= 2;
            }
        }
        BigInteger tmp(1);
        while (d--) {
            tmp *= 2;
        }
        a += b;
        a *= tmp;
        return a;
    }
    std::string toString_() const {
        std::string result = p.toString();
        if (q != 1) {
            result += '/';
            result += q.toString();
        }
        return result;
    }
    std::string asDecimal_(size_t precision = 0) const {
        BigInteger tmp(p);
        size_t precision_ = precision;
        size_t cnt0 = 0;
        bool flag = tmp.isMinus();
        tmp.setPositive();
        while (tmp < q && precision_) {
            tmp *= 10;
            ++cnt0;
            --precision_;
        }
        while (precision_ > 8) {
            precision_ -= 9;
            tmp *= 1000000000;
        }
        while (precision_) {
            --precision_;
            tmp *= 10;
        }
        tmp /= q;
        std::string result;
        result += tmp.toString();
        if (result[0] == '-') {
            result.erase(result.begin());
        }
        std::string zero;
        if (cnt0) {
            zero += "0.";
            while (--cnt0) {
                zero += '0';
            }
        }
        result = zero + result;
        if (result[0] != '0' && precision) {
            result.insert(result.end() - precision, '.');
        }
        if (flag) {
            result.insert(result.begin(), '-');
        }
        return result;
    }
};

bool operator==(int x, const Rational &y) {
    return Rational(x) == y;
}
bool operator!=(int x, const Rational &y) {
    return Rational(x) != y;
}
bool operator<(int x, const Rational &y) {
    return Rational(x) < y;
}
bool operator>(int x, const Rational &y) {
    return Rational(x) > y;
}
bool operator<=(int x, const Rational &y) {
    return Rational(x) <= y;
}
bool operator>=(int x, const Rational &y) {
    return Rational(x) >= y;
}
Rational operator+(int x, const Rational &other) {
    return Rational(x) += other;
}
Rational operator-(int x, const Rational &other) {
    return Rational(x) -= other;
}
Rational operator*(int x, const Rational &other) {
    return Rational(x) *= other;
}
Rational operator/(int x, const Rational &other) {
    return Rational(x) /= other;
}
Rational operator+(const BigInteger &x, const Rational &other) {
    return Rational(x) += other;
}
Rational operator-(const BigInteger &x, const Rational &other) {
    return Rational(x) -= other;
}
Rational operator*(const BigInteger &x, const Rational &other) {
    return Rational(x) *= other;
}
Rational operator/(const BigInteger &x, const Rational &other) {
    return Rational(x) /= other;
}