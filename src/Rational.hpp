#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include "Field.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <sstream>

class Rational : public Field<Rational> {

public:
    Rational(int64_t numerator = 0, int64_t denominator = 1)
        : _numerator(numerator), _denominator(denominator) {
        if (denominator == 0) {
            throw std::invalid_argument("Denominator cannot be zero");
        }
        _simplify();
    }

    Rational(const Rational& other) = default;

    Rational(const std::string& str) {
        size_t slashPos = str.find('/');
        size_t pos = 0;
        try {
            if (slashPos == std::string::npos) {
                _numerator = std::stoll(str, &pos);
                _denominator = 1;

                if (pos != str.size()) {
                    throw std::invalid_argument("Not an integer");
                }
            }
            else {
                std::string numeratorStr = str.substr(0, slashPos);
                std::string denominatorStr = str.substr(slashPos + 1);

                if (denominatorStr[0] == '+' || denominatorStr[0] == '-') {
                    throw std::invalid_argument("Denominator contains + or -");
                }

                _numerator = std::stoll(numeratorStr, &pos);
                if (pos != numeratorStr.size()) {
                    throw std::invalid_argument("Numerator not an integer");
                }

                _denominator = std::stoll(denominatorStr, &pos);
                if (pos != denominatorStr.size()) {
                    throw std::invalid_argument("Denominator not an integer");
                }

                if (_denominator == 0) {
                    throw std::invalid_argument("Denominator cannot be zero");
                }
                _simplify();
            }
        }
        catch (const std::exception& e) {
            throw std::invalid_argument("Invalid rational number format");
        }
    }

    Rational operator+(const Rational& other) const override {
        if (_denominator == other._denominator) {
            return Rational(_numerator + other._numerator, _denominator, false);
        }

        int64_t resultNumerator = _numerator * other._denominator + other._numerator * _denominator;
        int64_t resultDenominator = _denominator * other._denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    Rational operator-(const Rational& other) const override {
        if (_denominator == other._denominator) {
            return Rational(_numerator - other._numerator, _denominator, false);
        }

        int64_t resultNumerator = _numerator * other._denominator - other._numerator * _denominator;
        int64_t resultDenominator = _denominator * other._denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    Rational operator*(const Rational& other) const override {
        if (_numerator == 0 || other._numerator == 0) {
            return Rational(0, 1, false);
        }

        //  Cross-simplification to reduce intermediate results
        int64_t gcd1 = std::gcd(std::abs(_numerator), std::abs(other._denominator));
        int64_t gcd2 = std::gcd(std::abs(other._numerator), std::abs(_denominator));

        int64_t num1 = _numerator / gcd1;
        int64_t den1 = _denominator / gcd2;
        int64_t num2 = other._numerator / gcd2;
        int64_t den2 = other._denominator / gcd1;

        return Rational(num1 * num2, den1 * den2, false); 
    }

    Rational operator/(const Rational& other) const override {
        if (other._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        return *this * other.reciprocal();
    }

    Rational& operator+=(const Rational& other) override {
        if (_denominator == other._denominator) {
            _numerator += other._numerator;
        }
        else {
            _numerator = _numerator * other._denominator + other._numerator * _denominator;
            _denominator *= other._denominator;
        }
        _simplify();
        return *this;
    }

    Rational& operator-=(const Rational& other) override {
        if (_denominator == other._denominator) {
            _numerator -= other._numerator;
        }
        else {
            _numerator = _numerator * other._denominator - other._numerator * _denominator;
            _denominator *= other._denominator;
        }
        _simplify();
        return *this;
    }

    Rational& operator*=(const Rational& other) override {
        if (_numerator == 0 || other._numerator == 0) {
            _numerator = 0;
            _denominator = 1;
            return *this;
        }

        _numerator *= other._numerator;
        _denominator *= other._denominator;
        _simplify();
        return *this;
    }

    Rational& operator/=(const Rational& other) override {
        if (other._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }

        if(*this == other) {
            _numerator = 1;
            _denominator = 1;
            return *this;
        }

        _numerator *= other._denominator;
        _denominator *= other._numerator;
        _simplify();
        return *this;
    }

    Rational operator+() const override {
        return *this;
    }

    Rational operator-() const override {
        return Rational(-_numerator, _denominator, false); //  Skip simplification
    }

    bool operator==(const Rational& other) const override {
        if (_denominator == other._denominator) {
            return _numerator == other._numerator;
        }
        return _numerator * other._denominator == _denominator * other._numerator;
    }

    bool operator!=(const Rational& other) const override {
        return !(*this == other);
    }

    bool operator<(const Rational& other) const override {
        if (_denominator == other._denominator) {
            return _numerator < other._numerator;
        }

        //  Handle sign differences quickly
        bool thisNeg = _numerator < 0;
        bool otherNeg = other._numerator < 0;
        if (thisNeg && !otherNeg) {
            return true;
        }
        if (!thisNeg && otherNeg) {
            return false;
        }

        return _numerator * other._denominator < _denominator * other._numerator;
    }

    bool operator<=(const Rational& other) const override {
        return !(other < *this);
    }

    bool operator>(const Rational& other) const override {
        return other < *this;
    }

    bool operator>=(const Rational& other) const override {
        return !(*this < other);
    }

    Rational& operator=(const Rational& other) override = default;

    Rational additiveInverse() const override {
        return -(*this);
    }

    Rational multiplicativeInverse() const override {
        return reciprocal();
    }

    std::string toString() const override {
        if (_denominator == 1) {
            return std::to_string(_numerator);
        }
        return std::to_string(_numerator) + "/" + std::to_string(_denominator);
    }

    Rational power(int64_t exp) const override {
        if (exp == 0) {
            return Rational(1, 1, false);
        }
        if (exp < 0) {
            return reciprocal().power(-exp);
        }

        Rational result(1, 1, false);
        Rational base = *this;

        while (exp > 0) {
            if (exp & 1) {
                result *= base;
            }
            if (exp > 1) {
                base *= base;
            }
            exp >>= 1;
        }

        return result;
    }

    Rational operator+(int64_t other) const {
        if (other == 0) {
            return *this;
        }
        return Rational(_numerator + other * _denominator, _denominator, false);
    }

    friend Rational operator+(int64_t other, const Rational& r) {
        return r + other;
    }

    Rational& operator+=(int64_t other) {
        if (other != 0) {
            _numerator += other * _denominator;
            _simplify();
        }
        return *this;
    }

    Rational operator-(int64_t other) const {
        if (other == 0) {
            return *this;
        }
        return Rational(_numerator - other * _denominator, _denominator, false);
    }

    friend Rational operator-(int64_t other, const Rational& r) {
        return Rational(other * r._denominator - r._numerator, r._denominator);
    }

    Rational& operator-=(int64_t other) {
        if (other != 0) {
            _numerator -= other * _denominator;
            _simplify();
        }
        return *this;
    }

    Rational operator*(int64_t other) const {
        if (other == 0) {
            return Rational(0, 1, false);
        }
        if (other == 1) {
            return *this;
        }
        return Rational(_numerator * other, _denominator);
    }

    friend Rational operator*(int64_t other, const Rational& r) {
        return r * other;
    }

    Rational& operator*=(int64_t other) {
        if (other == 0) {
            _numerator = 0;
            _denominator = 1;
        }
        else if (other != 1) {
            _numerator *= other;
            _simplify();
        }
        return *this;
    }

    Rational operator/(int64_t other) const {
        if (other == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        if (other == 1) {
            return *this;
        }
        return Rational(_numerator, _denominator * other);
    }

    friend Rational operator/(int64_t other, const Rational& r) {
        if (r._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        return Rational(other * r._denominator, r._numerator);
    }

    Rational& operator/=(int64_t other) {
        if (other == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        if (other != 1) {
            _denominator *= other;
            _simplify();
        }
        return *this;
    }

    bool operator==(int64_t other) const {
        return _denominator == 1 && _numerator == other;
    }

    friend bool operator==(int64_t other, const Rational& r) {
        return r == other;
    }

    bool operator!=(int64_t other) const {
        return !(*this == other);
    }

    friend bool operator!=(int64_t other, const Rational& r) {
        return r != other;
    }

    bool operator<(int64_t other) const {
        return _numerator < other * _denominator;
    }

    friend bool operator<(int64_t other, const Rational& r) {
        return other * r._denominator < r._numerator;
    }

    bool operator>(int64_t other) const {
        return _numerator > other * _denominator;
    }

    friend bool operator>(int64_t other, const Rational& r) {
        return other * r._denominator > r._numerator;
    }

    bool operator<=(int64_t other) const {
        return !(*this > other);
    }

    friend bool operator<=(int64_t other, const Rational& r) {
        return !(other > r);
    }

    bool operator>=(int64_t other) const {
        return !(*this < other);
    }

    friend bool operator>=(int64_t other, const Rational& r) {
        return !(other < r);
    }

    friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
        os << r._numerator;
        if (r._denominator != 1) {
            os << "/" << r._denominator;
        }
        return os;
    }

    int64_t getNumerator() const {
        return _numerator;
    }

    int64_t getDenominator() const {
        return _denominator;
    }

    double toDouble() const {
        return static_cast<double>(_numerator) / static_cast<double>(_denominator);
    }

    float toFloat() const {
        return static_cast<float>(_numerator) / static_cast<float>(_denominator);
    }

    Rational abs() const {
        if (_numerator < 0) {
            return Rational(-_numerator, _denominator, false);
        }
        return *this;
    }

    Rational reciprocal() const {
        if (_numerator == 0) {
            throw std::invalid_argument("Cannot get reciprocal of zero");
        }
        if (_numerator < 0) {
            return Rational(-_denominator, -_numerator, false);
        }
        return Rational(_denominator, _numerator, false);
    }

    bool isInteger() const {
        return _denominator == 1;
    }

    const static Rational zero;
    const static Rational one;

private:
    int64_t _numerator;
    int64_t _denominator;

    Rational(int64_t numerator, int64_t denominator, bool simplify)
        : _numerator(numerator), _denominator(denominator) {
        if (simplify) {
            _simplify();
        }
    }

    void _simplify() {
        if (_numerator == 0) {
            _denominator = 1;
            return;
        }

        if (_denominator < 0) {
            _numerator = -_numerator;
            _denominator = -_denominator;
        }

        //  Only compute GCD if both values are reasonably large
        //  For small values, the GCD computation overhead might not be worth it
        if (std::abs(_numerator) > 1 || std::abs(_denominator) > 1) {
            int64_t gcd_val = std::gcd(std::abs(_numerator), std::abs(_denominator));
            if (gcd_val > 1) {
                _numerator /= gcd_val;
                _denominator /= gcd_val;
            }
        }
    }
};

#endif //  RATIONAL_HPP