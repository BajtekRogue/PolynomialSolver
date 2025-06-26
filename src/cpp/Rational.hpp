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
        else {
            _simplify();
        }
    }

    Rational(const Rational& other)
        : _numerator(other._numerator), _denominator(other._denominator) { }

    Rational(const std::string& str) {
        size_t slashPos = str.find('/');
        size_t pos = 0;
        try {
            if (slashPos == std::string::npos) {
                //  No slash, treat as integer
                _numerator = std::stoll(str, &pos);
                _denominator = 1;

                if (pos != str.size()) {
                    throw std::invalid_argument(
                        str + " cannot be converted into a Rational instance [not an integer]");
                }
            }
            else {
                std::string numeratorStr = str.substr(0, slashPos);
                std::string denominatorStr = str.substr(slashPos + 1);

                //  Denominator must be positive and without + sign
                if (denominatorStr[0] == '+' || denominatorStr[0] == '-') {
                    throw std::invalid_argument(str + " cannot be converted into a Rational "
                                                      "instance [denomiantor contains + or -]");
                }

                _numerator = std::stoll(numeratorStr, &pos);
                if (pos != numeratorStr.size()) {
                    throw std::invalid_argument(
                        str +
                        " cannot be converted into a Rational instance [numerator not an integer]");
                }

                _denominator = std::stoll(denominatorStr, &pos);
                if (pos != denominatorStr.size()) {
                    throw std::invalid_argument(str + " cannot be converted into a Rational "
                                                      "instance [denominator not an integer]");
                }

                if (_denominator == 0) {
                    throw std::invalid_argument("Denominator cannot be zero");
                }
                _simplify();
            }
        }


        catch (const std::exception& e) {
            throw std::invalid_argument(
                str + " cannot be converted into a Rational instance [not an integer]");
        }
    }

    Rational operator+(const Rational& other) const override {

        int64_t resultNumerator = _numerator * other._denominator + other._numerator * _denominator;
        int64_t resultDenominator = _denominator * other._denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    Rational operator-(const Rational& other) const override {
        int64_t resultNumerator = _numerator * other._denominator - other._numerator * _denominator;
        int64_t resultDenominator = _denominator * other._denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    Rational operator*(const Rational& other) const override {
        int64_t resultNumerator = _numerator * other._numerator;
        int64_t resultDenominator = _denominator * other._denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    Rational operator/(const Rational& other) const override {
        if (other._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        int64_t resultNumerator = _numerator * other._denominator;
        int64_t resultDenominator = _denominator * other._numerator;
        return Rational(resultNumerator, resultDenominator);
    }

    Rational& operator+=(const Rational& other) override {
        _numerator = _numerator * other._denominator + other._numerator * _denominator;
        _denominator = _denominator * other._denominator;
        _simplify();
        return *this;
    }

    Rational& operator-=(const Rational& other) override {
        _numerator = _numerator * other._denominator - other._numerator * _denominator;
        _denominator = _denominator * other._denominator;
        _simplify();
        return *this;
    }

    Rational& operator*=(const Rational& other) override {
        _numerator = _numerator * other._numerator;
        _denominator = _denominator * other._denominator;
        _simplify();
        return *this;
    }

    Rational& operator/=(const Rational& other) override {
        if (other._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        _numerator = _numerator * other._denominator;
        _denominator = _denominator * other._numerator;
        _simplify();
        return *this;
    }

    Rational operator+() const override {
        return *this;
    }

    Rational operator-() const override {
        return Rational(-_numerator, _denominator);
    }

    bool operator==(const Rational& other) const override {
        return _numerator == other._numerator && _denominator == other._denominator;
    }

    bool operator!=(const Rational& other) const override {
        return !(*this == other);
    }

    bool operator<(const Rational& other) const override {
        return _numerator *other._denominator < _denominator * other._numerator;
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

    Rational& operator=(const Rational& other) override {
        if (this != &other) {
            _numerator = other._numerator;
            _denominator = other._denominator;
        }
        return *this;
    }

    Rational additiveInverse() const override {
        return -(*this);
    }

    Rational multiplicativeInverse() const override {
        return reciprocal();
    }

    static Rational zero() {
        return Rational(0, 1);
    }

    static Rational one() {
        return Rational(1, 1);
    }

    bool isZero() const override {
        return _numerator == 0;
    }

    bool isOne() const override {
        return _numerator == 1 && _denominator == 1;
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << _numerator;
        if (_denominator != 1) {
            oss << "/" << _denominator;
        }
        return oss.str();
    }

    Rational power(int64_t exp) const override {
        if (exp == 0) {
            return Rational(1, 1);
        }
        if (exp < 0) {
            return reciprocal().power(-exp);
        }

        Rational result(1, 1);
        Rational base = *this;

        while (exp > 0) {
            if (exp & 1) {
                result *= base;
            }
            base *= base;
            exp >>= 1;
        }

        return result;
    }

    Rational operator+(int64_t other) const {
        int64_t resultNumerator = _numerator + other * _denominator;
        int64_t resultDenominator = _denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    friend Rational operator+(int64_t other, const Rational& r) {
        return r + other;
    }

    Rational& operator+=(int64_t other) {
        _numerator = _numerator + other * _denominator;
        _simplify();
        return *this;
    }

    Rational operator-(int64_t other) const {
        int64_t resultNumerator = _numerator - other * _denominator;
        int64_t resultDenominator = _denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    friend Rational operator-(int64_t other, const Rational& r) {
        return Rational(other) - r;
    }

    Rational& operator-=(int64_t other) {
        _numerator = _numerator - other * _denominator;
        _simplify();
        return *this;
    }

    Rational operator*(int64_t other) const {
        int64_t resultNumerator = _numerator * other;
        int64_t resultDenominator = _denominator;
        return Rational(resultNumerator, resultDenominator);
    }

    friend Rational operator*(int64_t other, const Rational& r) {
        return r * other;
    }

    Rational& operator*=(int64_t other) {
        _numerator = _numerator * other;
        _simplify();
        return *this;
    }

    Rational operator/(int64_t other) const {
        if (other == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        int64_t resultNumerator = _numerator;
        int64_t resultDenominator = _denominator * other;
        return Rational(resultNumerator, resultDenominator);
    }

    friend Rational operator/(int64_t other, const Rational& r) {
        return Rational(other * r.getDenominator(), r.getNumerator());
    }

    Rational& operator/=(int64_t other) {
        if (other == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        _denominator = _denominator * other;
        _simplify();
        return *this;
    }

    bool operator==(int64_t other) const {
        return _numerator == other && _denominator == 1;
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


    friend bool operator<(int64_t other, const Rational& r) {
        return Rational(other) < r;
    }

    bool operator<(int64_t other) const {
        return _numerator < other * _denominator;
    }


    bool operator>(int64_t other) const {
        return other < *this;
    }

    friend bool operator>(int64_t other, const Rational& r) {
        return r < other;
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
        return Rational(std::abs(_numerator), _denominator);
    }

    Rational reciprocal() const {
        if (_numerator == 0) {
            throw std::invalid_argument("Cannot get reciprocal of zero");
        }
        return Rational(_denominator, _numerator);
    }

    bool isInteger() const {
        return _denominator == 1;
    }

private:
    int64_t _numerator;
    int64_t _denominator;

    void _simplify() {
        if (_numerator == 0) {
            _denominator = 1;
            return;
        }

        if (_denominator < 0) {
            //  Make the denominator positive for consistency
            _numerator = -_numerator;
            _denominator = -_denominator;
        }

        int64_t gcd = std::gcd(std::abs(_numerator), std::abs(_denominator));
        _numerator /= gcd;
        _denominator /= gcd;
    }
};

#endif //  RATIONAL_HPP