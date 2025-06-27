#ifndef UNIVARIATE_POLYNOMIAL_HPP
#define UNIVARIATE_POLYNOMIAL_HPP

#include "Field.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

/**
 * Represents polynomials as `F[x]` where `F` is any field
 * Coefficients are stored in ascending order of powers
 */
template<typename F> class UnivariatePolynomial {
    static_assert(std::is_base_of_v<Field<F>, F>, "F must be derived from Field<F>");

public:
    UnivariatePolynomial() : _coefficients{F::zero} { }

    explicit UnivariatePolynomial(const F& constant) : _coefficients{constant} {
        _removeLeadingZeros();
    }

    explicit UnivariatePolynomial(const std::vector<F>& coeffs) : _coefficients(coeffs) {
        if (_coefficients.empty()) {
            _coefficients.push_back(F::zero);
        }
        _removeLeadingZeros();
    }

    explicit UnivariatePolynomial(std::vector<F>&& coeffs) : _coefficients(std::move(coeffs)) {
        if (_coefficients.empty()) {
            _coefficients.push_back(F::zero);
        }
        _removeLeadingZeros();
    }

    UnivariatePolynomial(const UnivariatePolynomial& other) : _coefficients(other._coefficients) { }

    UnivariatePolynomial(UnivariatePolynomial&& other) noexcept
        : _coefficients(std::move(other._coefficients)) { }

    UnivariatePolynomial& operator=(const UnivariatePolynomial& other) {
        if (this != &other) {
            _coefficients = other._coefficients;
        }
        return *this;
    }

    UnivariatePolynomial& operator=(UnivariatePolynomial&& other) noexcept {
        if (this != &other) {
            _coefficients = std::move(other._coefficients);
        }
        return *this;
    }

    int degree() const {
        return _coefficients.size() - 1;
    }

    const F& operator[](int index) const {
        return index >= _coefficients.size() ? F::zero : _coefficients[index];
    }

    F& operator[](int index) {
        if (index >= _coefficients.size()) {
            _coefficients.resize(index + 1, F::zero);
        }
        return _coefficients[index];
    }

    const F& leadingCoefficient() const {
        return _coefficients.back();
    }

    F& leadingCoefficient() {
        return _coefficients.back();
    }

    const std::vector<F>& getCoefficients() const {
        return _coefficients;
    }

    UnivariatePolynomial operator+(const UnivariatePolynomial& other) const {
        const int thisSize = _coefficients.size();
        const int otherSize = other._coefficients.size();
        const int maxSize = std::max(thisSize, otherSize);
        std::vector<F> result(maxSize);

        //  Process common indices
        const int minSize = std::min(thisSize, otherSize);
        for (int i = 0; i < minSize; i++) {
            result[i] = _coefficients[i] + other._coefficients[i];
        }

        //  Copy remaining coefficients from the longer polynomial
        if (thisSize > minSize) {
            std::copy(_coefficients.begin() + minSize, _coefficients.end(),
                      result.begin() + minSize);
        }
        else if (otherSize > minSize) {
            std::copy(other._coefficients.begin() + minSize, other._coefficients.end(),
                      result.begin() + minSize);
        }

        return UnivariatePolynomial(std::move(result));
    }

    UnivariatePolynomial operator+(const F& scalar) const {
        UnivariatePolynomial result = *this;
        result._coefficients.front() = result._coefficients.front() + scalar;
        result._removeLeadingZeros();
        return result;
    }

    friend UnivariatePolynomial operator+(const F& scalar, const UnivariatePolynomial& poly) {
        return poly + scalar;
    }

    UnivariatePolynomial& operator+=(const UnivariatePolynomial& other) {
        const int thisSize = _coefficients.size();
        const int otherSize = other._coefficients.size();

        if (otherSize > thisSize) {
            _coefficients.resize(otherSize, F::zero);
        }

        const int minSize = std::min(thisSize, otherSize);
        for (int i = 0; i < minSize; i++) {
            _coefficients[i] += other._coefficients[i];
        }

        //  Copy remaining coefficients if other is longer
        if (otherSize > thisSize) {
            std::copy(other._coefficients.begin() + thisSize, other._coefficients.end(),
                      _coefficients.begin() + thisSize);
        }

        _removeLeadingZeros();
        return *this;
    }

    UnivariatePolynomial& operator+=(const F& scalar) {
        _coefficients[0] = _coefficients[0] + scalar;
        _removeLeadingZeros();
        return *this;
    }

    UnivariatePolynomial operator-(const UnivariatePolynomial& other) const {
        const int thisSize = _coefficients.size();
        const int otherSize = other._coefficients.size();
        const int maxSize = std::max(thisSize, otherSize);
        std::vector<F> result(maxSize);

        //  Process common indices
        const int minSize = std::min(thisSize, otherSize);
        for (int i = 0; i < minSize; i++) {
            result[i] = _coefficients[i] - other._coefficients[i];
        }

        //  Handle remaining coefficients
        if (thisSize > minSize) {
            std::copy(_coefficients.begin() + minSize, _coefficients.end(),
                      result.begin() + minSize);
        }
        else if (otherSize > minSize) {
            for (int i = minSize; i < otherSize; i++) {
                result[i] = -other._coefficients[i];
            }
        }

        return UnivariatePolynomial(std::move(result));
    }

    UnivariatePolynomial operator-(const F& scalar) const {
        UnivariatePolynomial result = *this;
        result._coefficients.front() = result._coefficients.front() - scalar;
        result._removeLeadingZeros();
        return result;
    }

    friend UnivariatePolynomial operator-(const F& scalar, const UnivariatePolynomial& poly) {
        UnivariatePolynomial result = -poly;
        result._coefficients.front() = result._coefficients.front() + scalar;
        result._removeLeadingZeros();
        return result;
    }

    UnivariatePolynomial& operator-=(const UnivariatePolynomial& other) {
        const int thisSize = _coefficients.size();
        const int otherSize = other._coefficients.size();

        if (otherSize > thisSize) {
            _coefficients.resize(otherSize, F::zero);
        }

        const int minSize = std::min(thisSize, otherSize);
        for (int i = 0; i < minSize; i++) {
            _coefficients[i] -= other._coefficients[i];
        }

        //  Negate remaining coefficients if other is longer
        for (int i = thisSize; i < otherSize; i++) {
            _coefficients[i] = -other._coefficients[i];
        }

        _removeLeadingZeros();
        return *this;
    }

    UnivariatePolynomial& operator-=(const F& scalar) {
        _coefficients.front() = _coefficients.front() - scalar;
        _removeLeadingZeros();
        return *this;
    }

    UnivariatePolynomial operator*(const UnivariatePolynomial& other) const {
        if (isZeroPolynomial() || other.isZeroPolynomial()) {
            return UnivariatePolynomial();
        }

        const int thisSize = _coefficients.size();
        const int otherSize = other._coefficients.size();
        const int resultSize = thisSize + otherSize - 1;
        std::vector<F> result(resultSize, F::zero);

        for (int i = 0; i < thisSize; i++) {
            const F& coeff_i = _coefficients[i];

            if (coeff_i != F::zero) { //  Skip zero coefficients
                for (int j = 0; j < otherSize; j++) {
                    result[i + j] += coeff_i * other._coefficients[j];
                }
            }
        }

        return UnivariatePolynomial(std::move(result));
    }

    UnivariatePolynomial operator*(const F& scalar) const {
        if (scalar == F::zero) {
            return UnivariatePolynomial();
        }

        std::vector<F> result;
        result.reserve(_coefficients.size());

        for (const F& coeff : _coefficients) {
            result.push_back(coeff * scalar);
        }

        return UnivariatePolynomial(std::move(result));
    }

    friend UnivariatePolynomial operator*(const F& scalar, const UnivariatePolynomial& poly) {
        return poly * scalar;
    }

    UnivariatePolynomial& operator*=(const UnivariatePolynomial& other) {
        *this = *this * other;
        return *this;
    }

    UnivariatePolynomial& operator*=(const F& scalar) {
        if (scalar == F::zero) {
            _coefficients = {F::zero};
        }
        else {
            for (F& coeff : _coefficients) {
                coeff *= scalar;
            }
        }
        _removeLeadingZeros();
        return *this;
    }

    UnivariatePolynomial operator/(const UnivariatePolynomial& other) const {
        return _divmod(other).first;
    }

    UnivariatePolynomial operator%(const UnivariatePolynomial& other) const {
        return _divmod(other).second;
    }

    UnivariatePolynomial& operator/=(const UnivariatePolynomial& other) {
        *this = *this / other;
        return *this;
    }

    UnivariatePolynomial& operator%=(const UnivariatePolynomial& other) {
        *this = *this % other;
        return *this;
    }

    UnivariatePolynomial operator/(const F& scalar) const {
        if (scalar == F::zero) {
            throw std::invalid_argument("Division by zero");
        }

        F inv = scalar.multiplicativeInverse();
        return *this * inv;
    }

    UnivariatePolynomial& operator/=(const F& scalar) {
        if (scalar == F::zero) {
            throw std::invalid_argument("Division by zero");
        }

        F inv = scalar.multiplicativeInverse();
        for (F& coeff : _coefficients) {
            coeff *= inv;
        }
        _removeLeadingZeros();
        return *this;
    }

    UnivariatePolynomial operator+() const {
        return *this;
    }

    UnivariatePolynomial operator-() const {
        std::vector<F> result;
        result.reserve(_coefficients.size());
        for (const F& coeff : _coefficients) {
            result.push_back(-coeff);
        }
        return UnivariatePolynomial(std::move(result));
    }

    bool operator==(const UnivariatePolynomial& other) const {
        return _coefficients == other._coefficients;
    }

    bool operator!=(const UnivariatePolynomial& other) const {
        return !(*this == other);
    }

    bool isZeroPolynomial() const {
        return _coefficients.size() == 1 && _coefficients.front() == F::zero;
    }

    bool isConstant() const {
        return _coefficients.size() == 1;
    }

    bool isMonic() const {
        return !isZeroPolynomial() && leadingCoefficient() == F::one;
    }

    //  Make polynomial monic (leading coefficient = 1)
    UnivariatePolynomial makeMonic() const {
        if (isZeroPolynomial()) {
            throw std::invalid_argument("Cannot make zero polynomial monic");
        }

        return *this / leadingCoefficient();
    }

    //  Evaluate polynomial at a given point
    F evaluate(const F& x) const {
        if (_coefficients.empty()) {
            return F::zero;
        }

        //  Horner's method for efficient evaluation
        F result = _coefficients.back();
        for (int i = _coefficients.size() - 2; i >= 0; i--) {
            result = result * x + _coefficients[i];
        }

        return result;
    }

    //  Power operation
    UnivariatePolynomial power(int exp) const {
        if (exp == 0) {
            return UnivariatePolynomial(F::one);
        }
        if (exp == 1) {
            return *this;
        }

        UnivariatePolynomial result(F::one);
        UnivariatePolynomial base = *this;

        while (exp > 0) {
            if (exp % 2 == 1) {
                result *= base;
            }
            base *= base;
            exp /= 2;
        }
        return result;
    }

    //  String representation
    std::string toString(const std::string& variable = "x") const {
        if (isZeroPolynomial()) {
            return "0";
        }

        std::ostringstream oss;
        bool isFirst = true;

        for (int i = _coefficients.size() - 1; i >= 0; i--) {
            const F& coeff = _coefficients[i];
            std::string coeffStr = coeff.toString();

            if (coeff == F::zero) {
                continue;
            }

            if (!isFirst) {
                if (coeffStr[0] == '-') {
                    oss << " - ";
                    coeffStr = coeffStr.substr(1);
                }
                else {
                    oss << " + ";
                }
            }
            else {
                if (coeffStr[0] == '-') {
                    oss << "-";
                    coeffStr = coeffStr.substr(1);
                }
                isFirst = false;
            }

            //  Handle coefficient
            if (i == 0 || coeff != F::one) {
                oss << coeff.toString();
            }

            //  Handle variable and power
            if (i > 0) {
                if (coeff != F::one) {
                    oss << "*";
                }
                oss << variable;
                if (i > 1) {
                    oss << _toSuperscript(i);
                }
            }
        }

        return oss.str();
    }

    //  Stream output
    friend std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial& poly) {
        return os << poly.toString();
    }

    UnivariatePolynomial<F> derivative() const {
        const int n = degree();
        std::vector<F> result(n);

        for (int i = 0; i < n; i++) {
            result[i] = _coefficients[i + 1] * (i + 1);
        }

        return UnivariatePolynomial<F>(result);
    }


private:
    std::vector<F> _coefficients;

    void _removeLeadingZeros() {
        while (_coefficients.size() > 1 && _coefficients.back() == F::zero) {
            _coefficients.pop_back();
        }
    }

    std::pair<UnivariatePolynomial, UnivariatePolynomial>
        _divmod(const UnivariatePolynomial& divisor) const {
        if (divisor.isZeroPolynomial()) {
            throw std::invalid_argument("Division by zero polynomial");
        }

        if (degree() < divisor.degree()) {
            return {UnivariatePolynomial(), *this};
        }

        UnivariatePolynomial dividend = *this;
        UnivariatePolynomial quotient;
        F leadingCoeffInv = divisor.leadingCoefficient().multiplicativeInverse();

        while (!dividend.isZeroPolynomial() && dividend.degree() >= divisor.degree()) {

            F coeffRatio = dividend.leadingCoefficient() * leadingCoeffInv;
            int degreeDiff = dividend.degree() - divisor.degree();

            std::vector<F> monomialCoeffs(degreeDiff + 1, F::zero);
            monomialCoeffs[degreeDiff] = coeffRatio;
            UnivariatePolynomial monomial(std::move(monomialCoeffs));

            quotient += monomial;
            dividend -= divisor * monomial;
        }

        return {quotient, dividend};
    }

    std::string _toSuperscript(int num) const {
        const static std::map<char, std::string> superscripts = {
            {'0', "⁰"},
            {'1', "¹" },
            {'2', "²" },
            {'3', "³" },
            {'4', "⁴"},
            {'5', "⁵"},
            {'6', "⁶"},
            {'7', "⁷"},
            {'8', "⁸"},
            {'9', "⁹"}
        };
        std::string numStr = std::to_string(num);
        std::string result;
        for (char digit : numStr) {
            result += superscripts.at(digit);
        }
        return result;
    }
};

//  Helper function to create polynomials from initializer lists
template<typename F> UnivariatePolynomial<F> makePolynomial(std::initializer_list<F> coeffs) {
    return UnivariatePolynomial<F>(std::vector<F>(coeffs));
}

//  Helper function to create monomial x^n with coefficient c
template<typename F> UnivariatePolynomial<F> makeMonomial(const F& coefficient, int degree) {
    std::vector<F> coeffs(degree + 1, F::zero);
    coeffs[degree] = coefficient;
    return UnivariatePolynomial<F>(std::move(coeffs));
}

#endif //  UNIVARIATE_POLYNOMIAL_HPP