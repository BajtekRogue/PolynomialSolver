#ifndef BIG_RATIONAL_HPP
#define BIG_RATIONAL_HPP

#include "Field.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>

using BigInt = boost::multiprecision::cpp_int;

class BigRational : public Field<BigRational> {

public:
    BigRational(const BigInt& numerator = 0, const BigInt& denominator = 1)
        : _numerator(numerator), _denominator(denominator) {
        if (denominator == 0) {
            throw std::invalid_argument("Denominator cannot be zero");
        }
        _simplify();
    }

    BigRational(int64_t numerator, int64_t denominator = 1)
        : _numerator(numerator), _denominator(denominator) {
        if (denominator == 0) {
            throw std::invalid_argument("Denominator cannot be zero");
        }
        _simplify();
    }

    BigRational(const BigRational& other) = default;

    BigRational(const std::string& str) {
        size_t slashPos = str.find('/');
        try {
            if (slashPos == std::string::npos) {
                _numerator = BigInt(str);
                _denominator = 1;
            }
            else {
                std::string numeratorStr = str.substr(0, slashPos);
                std::string denominatorStr = str.substr(slashPos + 1);

                if (denominatorStr[0] == '+' || denominatorStr[0] == '-') {
                    throw std::invalid_argument("Denominator contains + or -");
                }

                _numerator = BigInt(numeratorStr);
                _denominator = BigInt(denominatorStr);

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

    BigRational operator+(const BigRational& other) const override {
        if (_denominator == other._denominator) {
            return BigRational(_numerator + other._numerator, _denominator);
        }

        BigInt resultNumerator = _numerator * other._denominator + other._numerator * _denominator;
        BigInt resultDenominator = _denominator * other._denominator;
        return BigRational(resultNumerator, resultDenominator);
    }

    BigRational operator-(const BigRational& other) const override {
        if (_denominator == other._denominator) {
            return BigRational(_numerator - other._numerator, _denominator);
        }

        BigInt resultNumerator = _numerator * other._denominator - other._numerator * _denominator;
        BigInt resultDenominator = _denominator * other._denominator;
        return BigRational(resultNumerator, resultDenominator);
    }

    BigRational operator*(const BigRational& other) const override {
        if (_numerator == 0 || other._numerator == 0) {
            return BigRational(0, 1);
        }

        //  Cross-simplification to reduce intermediate results
        BigInt gcd1 = boost::multiprecision::gcd(boost::multiprecision::abs(_numerator),
                                                 boost::multiprecision::abs(other._denominator));
        BigInt gcd2 = boost::multiprecision::gcd(boost::multiprecision::abs(other._numerator),
                                                 boost::multiprecision::abs(_denominator));

        BigInt num1 = _numerator / gcd1;
        BigInt den1 = _denominator / gcd2;
        BigInt num2 = other._numerator / gcd2;
        BigInt den2 = other._denominator / gcd1;

        return BigRational(num1 * num2, den1 * den2, false); 
    }

    BigRational operator/(const BigRational& other) const override {
        if (other._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        return *this * other.reciprocal();
    }

    BigRational& operator+=(const BigRational& other) override {
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

    BigRational& operator-=(const BigRational& other) override {
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

    BigRational& operator*=(const BigRational& other) override {
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

    BigRational& operator/=(const BigRational& other) override {
        if (other._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }

        _numerator *= other._denominator;
        _denominator *= other._numerator;
        _simplify();
        return *this;
    }

    BigRational operator+() const override {
        return *this;
    }

    BigRational operator-() const override {
        return BigRational(-_numerator, _denominator, false); 
    }

    bool operator==(const BigRational& other) const override {
        if (_denominator == other._denominator) {
            return _numerator == other._numerator;
        }
        return _numerator * other._denominator == _denominator * other._numerator;
    }

    bool operator!=(const BigRational& other) const override {
        return !(*this == other);
    }

    bool operator<(const BigRational& other) const override {
        if (_denominator == other._denominator) {
            return _numerator < other._numerator;
        }

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

    bool operator<=(const BigRational& other) const override {
        return !(other < *this);
    }

    bool operator>(const BigRational& other) const override {
        return other < *this;
    }

    bool operator>=(const BigRational& other) const override {
        return !(*this < other);
    }

    BigRational& operator=(const BigRational& other) = default;

    BigRational additiveInverse() const override {
        return -(*this);
    }

    BigRational multiplicativeInverse() const override {
        return reciprocal();
    }

    std::string toString() const override {
        if (_denominator == 1) {
            return _numerator.str();
        }
        return _numerator.str() + "/" + _denominator.str();
    }

    BigRational power(int64_t exp) const override {
        if (exp == 0) {
            return BigRational(1, 1, false);
        }
        if (exp < 0) {
            return reciprocal().power(-exp);
        }

        BigRational result(1, 1, false);
        BigRational base = *this;

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

    BigRational operator+(int64_t other) const {
        if (other == 0) {
            return *this;
        }
        return BigRational(_numerator + other * _denominator, _denominator, false);
    }

    friend BigRational operator+(int64_t other, const BigRational& r) {
        return r + other;
    }

    BigRational& operator+=(int64_t other) {
        if (other != 0) {
            _numerator += other * _denominator;
            _simplify();
        }
        return *this;
    }

    BigRational operator-(int64_t other) const {
        if (other == 0) {
            return *this;
        }
        return BigRational(_numerator - other * _denominator, _denominator, false);
    }

    friend BigRational operator-(int64_t other, const BigRational& r) {
        return BigRational(other * r._denominator - r._numerator, r._denominator);
    }

    BigRational& operator-=(int64_t other) {
        if (other != 0) {
            _numerator -= other * _denominator;
            _simplify();
        }
        return *this;
    }

    BigRational operator*(int64_t other) const {
        if (other == 0) {
            return BigRational(0, 1, false);
        }
        if (other == 1) {
            return *this;
        }
        return BigRational(_numerator * other, _denominator);
    }

    friend BigRational operator*(int64_t other, const BigRational& r) {
        return r * other;
    }

    BigRational& operator*=(int64_t other) {
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

    BigRational operator/(int64_t other) const {
        if (other == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        if (other == 1) {
            return *this;
        }
        return BigRational(_numerator, _denominator * other);
    }

    friend BigRational operator/(int64_t other, const BigRational& r) {
        if (r._numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        return BigRational(other * r._denominator, r._numerator);
    }

    BigRational& operator/=(int64_t other) {
        if (other == 0) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        if (other != 1) {
            _denominator *= other;
            _simplify();
        }
        return *this;
    }

    //  Optimized int64_t comparisons
    bool operator==(int64_t other) const {
        return _denominator == 1 && _numerator == other;
    }

    friend bool operator==(int64_t other, const BigRational& r) {
        return r == other;
    }

    bool operator!=(int64_t other) const {
        return !(*this == other);
    }

    friend bool operator!=(int64_t other, const BigRational& r) {
        return r != other;
    }

    bool operator<(int64_t other) const {
        return _numerator < other * _denominator;
    }

    friend bool operator<(int64_t other, const BigRational& r) {
        return other * r._denominator < r._numerator;
    }

    bool operator>(int64_t other) const {
        return _numerator > other * _denominator;
    }

    friend bool operator>(int64_t other, const BigRational& r) {
        return other * r._denominator > r._numerator;
    }

    bool operator<=(int64_t other) const {
        return !(*this > other);
    }

    friend bool operator<=(int64_t other, const BigRational& r) {
        return !(other > r);
    }

    bool operator>=(int64_t other) const {
        return !(*this < other);
    }

    friend bool operator>=(int64_t other, const BigRational& r) {
        return !(other < r);
    }

    friend std::ostream& operator<<(std::ostream& os, const BigRational& r) {
        os << r._numerator;
        if (r._denominator != 1) {
            os << "/" << r._denominator;
        }
        return os;
    }

    const BigInt& getNumerator() const {
        return _numerator;
    }

    const BigInt& getDenominator() const {
        return _denominator;
    }

    double toDouble() const {
        return static_cast<double>(_numerator) / static_cast<double>(_denominator);
    }

    float toFloat() const {
        return static_cast<float>(_numerator) / static_cast<float>(_denominator);
    }

    BigRational abs() const {
        if (_numerator < 0) {
            return BigRational(-_numerator, _denominator, false);
        }
        return *this;
    }

    BigRational reciprocal() const {
        if (_numerator == 0) {
            throw std::invalid_argument("Cannot get reciprocal of zero");
        }
        if (_numerator < 0) {
            return BigRational(-_denominator, -_numerator, false);
        }
        return BigRational(_denominator, _numerator, false);
    }

    bool isInteger() const {
        return _denominator == 1;
    }

    const static BigRational zero;
    const static BigRational one;

private:
    BigInt _numerator;
    BigInt _denominator;

    BigRational(const BigInt& numerator, const BigInt& denominator, bool simplify)
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
        if (boost::multiprecision::abs(_numerator) > 1 ||
            boost::multiprecision::abs(_denominator) > 1) {
            BigInt gcd_val = boost::multiprecision::gcd(boost::multiprecision::abs(_numerator),
                                                        boost::multiprecision::abs(_denominator));
            if (gcd_val > 1) {
                _numerator /= gcd_val;
                _denominator /= gcd_val;
            }
        }
    }
};

#endif //  BIG_RATIONAL_HPP