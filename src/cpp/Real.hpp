#ifndef REAL_HPP
#define REAL_HPP

#include "Field.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <charconv>

class Real : public Field<Real> {
public:
    Real(double val = 0.0) : _value(val) { }

    Real(const Real& other) : _value(other._value) { }

    Real(const std::string& str) {
        try {
            size_t idx;
            _value = std::stod(str, &idx);
            if (idx != str.size()) {
                throw std::invalid_argument("");
            }
        }
        catch (const std::exception& e) {
            throw std::invalid_argument(str +
                                        " cannot be converted into a Real instance [not a float]");
        }
    }


    Real operator+(const Real& other) const override {
        return Real(_value + other._value);
    }

    Real operator-(const Real& other) const override {
        return Real(_value - other._value);
    }

    Real operator*(const Real& other) const override {
        return Real(_value * other._value);
    }

    Real operator/(const Real& other) const override {
        if (other.isZero()) {
            throw std::domain_error("Division by zero");
        }
        return Real(_value / other._value);
    }

    Real& operator+=(const Real& other) override {
        _value += other._value;
        return *this;
    }

    Real& operator-=(const Real& other) override {
        _value -= other._value;
        return *this;
    }

    Real& operator*=(const Real& other) override {
        _value *= other._value;
        return *this;
    }

    Real& operator/=(const Real& other) override {
        if (other.isZero()) {
            throw std::domain_error("Division by zero");
        }
        _value /= other._value;
        return *this;
    }

    Real operator+() const override {
        return *this;
    }

    Real operator-() const override {
        return Real(-_value);
    }

    bool operator==(const Real& other) const override {
        return std::abs(_value - other._value) < Real::epsilon;
    }

    bool operator!=(const Real& other) const override {
        return !(*this == other);
    }

    bool operator<(const Real& other) const override {
        return std::abs(_value - other._value) < Real::epsilon ? false : _value < other._value;
    }

    bool operator<=(const Real& other) const override {
        return !(other < *this);
    }

    bool operator>(const Real& other) const override {
        return other < *this;
    }

    bool operator>=(const Real& other) const override {
        return !(*this < other);
    }

    Real& operator=(const Real& other) override {
        _value = other._value;
        return *this;
    }

    Real additiveInverse() const override {
        return Real(-_value);
    }

    Real multiplicativeInverse() const override {
        if (isZero()) {
            throw std::domain_error("Zero has no multiplicative inverse");
        }
        return Real(1.0 / _value);
    }

    static Real zero() {
        return Real(0.0);
    }

    static Real one() {
        return Real(1.0);
    }

    bool isZero() const override {
        return std::abs(_value) < Real::epsilon;
    }

    bool isOne() const override {
        return std::abs(_value - 1.0) < Real::epsilon;
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss.precision(12);
        oss << _value;
        return oss.str();
    }

    Real power(int64_t exp) const override {
        return Real(std::pow(_value, static_cast<double>(exp)));
    }

	double getValue() {
        return _value;
    }

    static void setEpsilon(double eps) {
        epsilon = eps;
    }

    static double epsilon;
    const static Real null;

private:
    double _value;
};


#endif //  REAL_HPP
