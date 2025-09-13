#ifndef REAL_HPP
#define REAL_HPP

#include "Field.hpp"

#include <charconv>
#include <cmath>
#include <sstream>
#include <stdexcept>

class Real : public Field<Real> {
public:
    Real(double val = 0.0) : _value(val) { }

    Real(const Real& other) : _value(other._value) { }

    Real(const std::string& str) {
        try {
            size_t pos;
            _value = std::stod(str, &pos);
            if (pos != str.size()) {
                throw std::invalid_argument("");
            }
        }
        catch (const std::exception& e) {
            throw std::invalid_argument("Not a float");
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
        if (other == Real::zero) {
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
        if (other == Real::zero) {
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
        if (std::abs(_value) < Real::epsilon) {
            throw std::domain_error("Zero has no multiplicative inverse");
        }
        return Real(1.0 / _value);
    }

    std::string toString() const override {
        if (std::abs(_value) < Real::epsilon) {
            return "0";
        }

        //  Check if value is close to an integer
        double rounded = std::round(_value);
        if (std::abs(_value - rounded) < Real::epsilon) {
            std::ostringstream oss;
            oss << static_cast<int64_t>(rounded);
            return oss.str();
        }

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

    const static Real zero;
    const static Real one;

private:
    double _value;
};


#endif //  REAL_HPP
