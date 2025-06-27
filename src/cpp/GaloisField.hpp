#ifndef GALOISFIELD_HPP
#define GALOISFIELD_HPP

#include "Field.hpp"

#include <map>
#include <sstream>
#include <stdexcept>

/**
 * Implementation of a Galois Field `F_p` where `p` is prime.
 * Elements are represented as integers modulo in the range `[0; p-1]`.
 */
class GaloisField : public Field<GaloisField> {

public:
    GaloisField(int64_t val = 0) {
        _value = _normalize(val);
    }

    GaloisField(const GaloisField& other) : _value(other._value) { }

    GaloisField(const std::string& str) {
        try {
            size_t pos = 0;
            _value = _normalize(std::stoll(str, &pos));

            if (pos != str.size()) {
                throw std::invalid_argument("Not an integer");
            }
        }
        catch (const std::exception& e) {
            throw std::invalid_argument("Not an integer");
        }
    }

    GaloisField operator+(const GaloisField& other) const override {
        return GaloisField((_value + other._value) % prime);
    }

    GaloisField operator-(const GaloisField& other) const override {
        return GaloisField(_normalize(_value - other._value));
    }

    GaloisField operator*(const GaloisField& other) const override {
        return GaloisField((_value * other._value) % prime);
    }

    GaloisField operator/(const GaloisField& other) const override {
        if (other._value == 0) {
            throw std::invalid_argument("Division by zero");
        }
        return GaloisField((_value * _modInverse(other._value, prime)) % prime);
    }

    GaloisField& operator+=(const GaloisField& other) override {
        _value = (_value + other._value) % prime;
        return *this;
    }

    GaloisField& operator-=(const GaloisField& other) override {
        _value = _normalize(_value - other._value);
        return *this;
    }

    GaloisField& operator*=(const GaloisField& other) override {
        _value = (_value * other._value) % prime;
        return *this;
    }

    GaloisField& operator/=(const GaloisField& other) override {
        if (other._value == 0) {
            throw std::invalid_argument("Division by zero");
        }
        _value = (_value * _modInverse(other._value, prime)) % prime;
        return *this;
    }

    GaloisField operator+() const override {
        return *this;
    }

    GaloisField operator-() const override {
        return GaloisField(_value == 0 ? 0 : prime - _value);
    }

    bool operator==(const GaloisField& other) const override {
        return _value == other._value;
    }

    bool operator!=(const GaloisField& other) const override {
        return _value != other._value;
    }

    bool operator<(const GaloisField& other) const override {
        return _value < other._value;
    }

    bool operator<=(const GaloisField& other) const override {
        return !(other < *this);
    }

    bool operator>(const GaloisField& other) const override {
        return other < *this;
    }

    bool operator>=(const GaloisField& other) const override {
        return !(*this < other);
    }

    GaloisField& operator=(const GaloisField& other) override {
        if (this != &other) {
            _value = other._value;
        }
        return *this;
    }

    GaloisField additiveInverse() const override {
        return -(*this);
    }

    GaloisField multiplicativeInverse() const override {
        if (_value == 0) {
            throw std::invalid_argument("Cannot compute multiplicative inverse of zero");
        }
        return GaloisField(_modInverse(_value, prime));
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << _value;
        return oss.str();
    }

    GaloisField power(int64_t exp) const override {
        if (exp == 0) {
            return one;
        }
        if (exp < 0) {
            return multiplicativeInverse().power(-exp);
        }
        return GaloisField(_modPow(_value, exp, prime));
    }

    int64_t toInt() const {
        return _value;
    }

    static bool setPrime(int64_t p) {
        if (p <= 1) {
            return false;
        }

        if (p == 2) {
            prime = p;
            return true;
        }

        if (p % 2 == 0) {
            return false;
        }

        for (int64_t i = 3; i * i <= p; i += 2) {
            if (p % i == 0) {
                return false;
            }
        }
        prime = p;
        return true;
    }

    static int64_t prime;

    const static GaloisField zero;
    const static GaloisField one;

private:
    int64_t _value;

    static int64_t _modPow(int64_t base, int64_t exp, int64_t mod) {
        int64_t result = 1;
        base %= mod;
        while (exp > 0) {
            if (exp & 1) {
                result = (result * base) % mod;
            }
            base = (base * base) % mod;
            exp >>= 1;
        }
        return result;
    }

    //  Helper function to compute modular multiplicative inverse using Fermat's little theorem
    static int64_t _modInverse(int64_t a, int64_t p) {
        if (a == 0) {
            throw std::invalid_argument("Cannot compute multiplicative inverse of zero");
        }
        return _modPow(a, p - 2, p);
    }

    //  Normalize value to `[0; p-1]`
    int64_t _normalize(int64_t val) const {
        val %= prime;
        if (val < 0) {
            val += prime;
        }
        return val;
    }
};


#endif //  GALOISFIELD_HPP