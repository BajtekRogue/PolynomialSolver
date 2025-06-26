#ifndef MULTIVARIATE_POLYNOMIAL_HPP
#define MULTIVARIATE_POLYNOMIAL_HPP

#include "Field.hpp"
#include "Monomial.hpp"
#include "MonomialOrders.hpp"

#include <complex>
#include <iostream>
#include <map>

/**
 * @brief Represents a multivariable polynomial over a field `F`. Stores it as a map of monomials
 * and their corresponding coefficients.
 */
template<typename F> class MultivariatePolynomial {
    static_assert(std::is_base_of_v<Field<F>, F>, "F must be derived from Field<F>");

public:
    MultivariatePolynomial() { }

    MultivariatePolynomial(F constant) {
        if (constant != F::zero()) {
            _coefficients[Monomial()] = constant;
        }
    }

    MultivariatePolynomial(std::map<Monomial, F> coefficients) {
        for (auto& [monomial, coefficient] : coefficients) {
            if (coefficient != F::zero()) {
                _coefficients[std::move(monomial)] = std::move(coefficient);
            }
        }
    }

    MultivariatePolynomial(const MultivariatePolynomial<F>& other)
        : _coefficients(other._coefficients) { }

    MultivariatePolynomial(MultivariatePolynomial<F>&& other) noexcept
        : _coefficients(std::move(other._coefficients)) { }

    explicit MultivariatePolynomial(const std::string& str) {
        std::string s = str;
        s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());

        if (s.empty()) {
            throw std::invalid_argument(
                str + " cannot be converted into a MultivariatePolynomial instance [empty string]");
        }

        if (s == "0") {
            return;
        }

        //  Split into terms by + and - signs
        std::vector<std::string> terms;
        std::string currentTerm;

        //  Handle the first character if it's a sign
        size_t start = 0;
        if (s[0] == '+' || s[0] == '-') {
            currentTerm = s[0];
            start = 1;
        }

        //  Validate term structure - should not have consecutive operators
        bool hasConsecutiveOperators = false;
        char lastChar = '\0';
        for (char c : s) {
            if ((c == '+' || c == '-') && (lastChar == '+' || lastChar == '-')) {
                hasConsecutiveOperators = true;
                break;
            }
            lastChar = c;
        }

        if (hasConsecutiveOperators) {
            throw std::invalid_argument(str + " cannot be converted into a MultivariatePolynomial "
                                              "instance [consecative operators]");
        }

        for (size_t i = start; i < s.size(); ++i) {
            if (s[i] == '+' || s[i] == '-') {
                if (!currentTerm.empty()) {
                    terms.push_back(currentTerm);
                }
                currentTerm = s[i];
            }
            else {
                currentTerm += s[i];
            }
        }

        if (!currentTerm.empty()) {
            terms.push_back(currentTerm);
        }

        //  Process each term
        for (const std::string& term : terms) {
            if (term.empty()) {
                continue;
            }

            std::string coeffStr;
            std::string monomialStr;

            //  Find where coefficient ends and monomial begins
            size_t i = 0;

            //  Handle sign
            if (term[0] == '+' || term[0] == '-') {
                coeffStr += term[0];
                i = 1;
            }


            while (i < term.size()) {
                char c = term[i];

                if (std::isalpha(c)) {
                    //  Found a variable, coefficient part is done
                    break;
                }
                else if (std::isdigit(c) || c == '.' || c == '/') {
                    coeffStr += c;
                }
                else {
                    //  Other characters (like operators) mean we're in monomial part
                    break;
                }

                i++;
            }

            //  Extract monomial part (rest of the string)
            monomialStr = term.substr(i);

            //  If monomials is empty need to check coeff being + or -
            if (monomialStr.empty() && (coeffStr == "+" || coeffStr == "-")) {
                throw std::invalid_argument("No number");
            }

            //  Handle special cases for coefficient
            if (coeffStr.empty() || coeffStr == "+") {
                coeffStr = "1";
            }
            else if (coeffStr == "-") {
                coeffStr = "-1";
            }

            //  Handle case where there's no monomial (constant term)
            if (monomialStr.empty()) {
                monomialStr = ""; //  Represents constant monomial
            }


            if (coeffStr.empty()) {
                throw std::invalid_argument("Empty coefficient in term: " + term);
            }

            //  Create coefficient and monomial objects
            try {
                F coefficient(coeffStr);
                Monomial monomial(monomialStr);

                if (coefficient == F::zero()) {
                    continue;
                }

                //  Add to the polynomial (accumulate if monomial already exists)
                auto it = _coefficients.find(monomial);
                if (it != _coefficients.end()) {
                    //  Monomial already exists, add coefficients properly
                    F existingCoeff = it->second;
                    F newCoeff = existingCoeff + coefficient;
                    it->second = newCoeff;
                }
                else {
                    _coefficients[monomial] = coefficient;
                }
            }
            catch (const std::exception& e) {
                //  Handle parsing errors
                throw std::invalid_argument("Failed to parse term: " + term + " bc " + e.what());
            }
        }
    }

    std::map<Monomial, F> getCoefficients() const {
        return _coefficients;
    }

    MultivariatePolynomial<F>& operator=(const MultivariatePolynomial<F>& other) {
        if (this != &other) {
            _coefficients = other._coefficients;
            if (other._validLeadingTerm) {
                _validLeadingTerm = true;
                _cachedLeadingMonomial = other._cachedLeadingMonomial;
                _cachedLeadingCoefficient = other._cachedLeadingCoefficient;
                _cachedOrder = other._cachedOrder;
            }
            else {
                _validLeadingTerm = false;
            }
        }
        return *this;
    }

    MultivariatePolynomial operator+(const MultivariatePolynomial<F>& other) const {
        std::map<Monomial, F> resultCoefficients = _coefficients;

        for (const auto& [monomial, coefficient] : other._coefficients) {
            resultCoefficients[monomial] += coefficient;

            if (resultCoefficients[monomial] == F::zero()) {
                resultCoefficients.erase(monomial);
            }
        }
        return MultivariatePolynomial(resultCoefficients);
    }

    MultivariatePolynomial operator+(F other) const {
        std::map<Monomial, F> resultCoefficients = _coefficients;

        resultCoefficients[Monomial()] += other;
        if (resultCoefficients[Monomial()] == F::zero()) {
            resultCoefficients.erase(Monomial());
        }

        return MultivariatePolynomial(resultCoefficients);
    }

    friend MultivariatePolynomial operator+(F other, const MultivariatePolynomial<F>& p) {
        return p + other;
    }

    MultivariatePolynomial operator+=(const MultivariatePolynomial<F>& other) {
        for (const auto& [monomial, coefficient] : other._coefficients) {
            _coefficients[monomial] += coefficient;

            if (_coefficients[monomial] == F::zero()) {
                _coefficients.erase(monomial);
            }
        }

        _validLeadingTerm = false;
        return *this;
    }

    MultivariatePolynomial operator+=(F other) {
        _coefficients[Monomial()] += other;
        _validLeadingTerm = false;
        return *this;
    }

    MultivariatePolynomial operator-(const MultivariatePolynomial<F>& other) const {
        std::map<Monomial, F> resultCoefficients = _coefficients;

        for (const auto& [monomial, coefficient] : other._coefficients) {
            resultCoefficients[monomial] -= coefficient;
            if (resultCoefficients[monomial] == F::zero()) {
                resultCoefficients.erase(monomial);
            }
        }

        return MultivariatePolynomial(resultCoefficients);
    }

    MultivariatePolynomial operator-(F other) const {
        std::map<Monomial, F> resultCoefficients = _coefficients;

        resultCoefficients[Monomial()] -= other;
        if (resultCoefficients[Monomial()] == F::zero()) {
            resultCoefficients.erase(Monomial());
        }

        return MultivariatePolynomial(resultCoefficients);
    }

    friend MultivariatePolynomial operator-(F other, const MultivariatePolynomial<F>& p) {
        return -(p - other);
    }

    MultivariatePolynomial operator-=(const MultivariatePolynomial<F>& other) {
        for (const auto& [monomial, coefficient] : other._coefficients) {
            _coefficients[monomial] -= coefficient;
            if (_coefficients[monomial] == F::zero()) {
                _coefficients.erase(monomial);
            }
        }

        _validLeadingTerm = false;
        return *this;
    }

    MultivariatePolynomial operator-=(F other) {
        _coefficients[Monomial()] -= other;
        if (_coefficients[Monomial()] == F::zero()) {
            _coefficients.erase(Monomial());
        }

        _validLeadingTerm = false;
        return *this;
    }

    MultivariatePolynomial operator*(const MultivariatePolynomial<F>& other) const {
        std::map<Monomial, F> resultCoefficients;

        for (const auto& [monomial1, coefficient1] : _coefficients) {
            for (const auto& [monomial2, coefficient2] : other._coefficients) {

                Monomial monomial = monomial1 * monomial2;
                F coefficient = coefficient1 * coefficient2;

                if (coefficient != F::zero()) {
                    resultCoefficients[monomial] += coefficient;
                }
            }
        }
        return MultivariatePolynomial(resultCoefficients);
    }

    MultivariatePolynomial operator*(F other) const {

        if (other == F::zero()) {
            return MultivariatePolynomial();
        }

        std::map<Monomial, F> resultCoefficients;
        for (const auto& [monomial, coefficient] : _coefficients) {

            F newCoefficient = coefficient * other;
            if (newCoefficient != F::zero()) {
                resultCoefficients[monomial] = newCoefficient;
            }
        }

        return MultivariatePolynomial(resultCoefficients);
    }

    friend MultivariatePolynomial operator*(F other, const MultivariatePolynomial<F>& p) {
        return p * other;
    }

    MultivariatePolynomial operator*=(const MultivariatePolynomial<F>& other) {
        std::map<Monomial, F> resultCoefficients;

        for (const auto& [monomial1, coefficient1] : _coefficients) {
            for (const auto& [monomial2, coefficient2] : other._coefficients) {

                Monomial monomial = monomial1 * monomial2;
                F coefficient = coefficient1 * coefficient2;

                if (coefficient != F::zero()) {
                    resultCoefficients[monomial] += coefficient;
                }
            }
        }

        _validLeadingTerm = false;
        _coefficients = resultCoefficients;
        return *this;
    }

    MultivariatePolynomial operator*=(F other) {
        std::map<Monomial, F> resultCoefficients;
        for (auto& [monomial, coefficient] : _coefficients) {

            F newCoefficient = coefficient * other;
            if (newCoefficient != F::zero()) {
                resultCoefficients[monomial] = newCoefficient;
            }
        }

        _validLeadingTerm = false;
        _coefficients = resultCoefficients;
        return *this;
    }

    MultivariatePolynomial operator+() {
        return *this;
    }

    MultivariatePolynomial operator-() {
        std::map<Monomial, F> resultCoefficients;
        for (const auto& [monomial, coefficient] : _coefficients) {
            resultCoefficients[monomial] = -coefficient;
        }
        return MultivariatePolynomial(resultCoefficients);
    }

    friend std::ostream& operator<<(std::ostream& os, const MultivariatePolynomial& p) {
        os << p.toString();
        return os;
    }

    std::string toString() const {
        if (_coefficients.empty()) {
            return "0";
        }

        std::ostringstream oss;
        bool isFirst = true;

        //  Iterate in reverse order (highest degree first)
        for (auto it = _coefficients.rbegin(); it != _coefficients.rend(); ++it) {
            const Monomial& monomial = it->first;
            const F& coefficient = it->second;

            if (coefficient == F::zero()) {
                continue;
            }

            std::string monomStr = monomial.toString();
            std::string coeffStr = coefficient.toString();

            //  Handle the sign
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

            //  Handle coefficient printing
            if (coeffStr != "1" || monomStr == "1") {
                //  Don't print coefficient 1 unless it's a constant term
                if (coeffStr == "1" && monomStr == "1") {
                    oss << "1";
                }
                else if (coeffStr != "1") {
                    oss << coeffStr;
                }
            }

            //  Handle monomial printing
            if (!monomStr.empty() && monomStr != "1") {
                if (coeffStr != "1" && !coeffStr.empty()) {
                    oss << "·"; //  use middle dot for multiplication
                }
                oss << monomStr;
            }
        }

        return oss.str();
    }

    bool operator==(const MultivariatePolynomial<F>& other) const {
        return (*this - other).isZeroPolynomial();
    }

    bool operator==(F other) const {
        if (_coefficients.size() == 0) {
            return other == F::zero();
        }
        else {
            return (_coefficients.size() == 1 && _coefficients.begin()->first == Monomial() &&
                    _coefficients.begin()->second == other);
        }
    }

    bool operator!=(const MultivariatePolynomial<F>& other) const {
        return !(*this == other);
    }

    bool operator!=(F other) const {
        return !(other == *this);
    }

    MultivariatePolynomial operator^(int exp) const {
        if (exp < 0) {
            throw std::invalid_argument("Negative exponent is not allowed");
        }
        else if (exp == 0) {
            return MultivariatePolynomial<F>({
                {Monomial(), F::one()}
            });
        }

        MultivariatePolynomial<F> result = MultivariatePolynomial<F>({
            {Monomial(), F::one()}
        });
        MultivariatePolynomial<F> base = *this;
        while (exp > 0) {
            if (exp % 2 == 1) {
                result *= base;
            }
            base *= base;
            exp /= 2;
        }
        return result;
    }

    bool isZeroPolynomial() const {
        for (const auto& [_, coefficient] : _coefficients) {
            if (coefficient != F::zero()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Given map of variable values, evaluate the polynomial at the given point
     * @throws `std::invalid_argument` if a variable exists in polynomial but not in the values map
     */
    F evaluate(const std::map<char, F>& values) const {
        F result = F::zero();

        for (const auto& [monomial, coefficient] : _coefficients) {
            F term = coefficient;

            for (const auto& [var, exp] : monomial.getMonomial()) {

                if (values.find(var) == values.end()) {
                    throw std::invalid_argument(
                        "Variable '" + std::string(1, var) +
                        "' found in the polynomial but it's value is not provided");
                }
                term *= _power(values.at(var), exp);
            }
            result += term;
        }

        return result;
    }

    /**
     * @brief Calculate the total degree of the polynomial
     */
    int totalDegree() const {
        int result = 0;
        for (const auto& [monomial, _] : _coefficients) {
            result = std::max(result, monomial.getDegree());
        }
        return result;
    }

    /**
     * @brief Get the variables present in the polynomial
     */
    std::vector<char> getVariables() const {
        std::set<char> variables;
        for (const auto& [monomial, _] : _coefficients) {
            for (const auto& var : monomial.getVariables()) {
                variables.insert(var);
            }
        }
        return std::vector<char>(variables.begin(), variables.end());
    }

    /**
     * @brief Substitute a variable with a value in the polynomial
     * @throws std::invalid_argument if the variable is not found in the polynomial
     */
    MultivariatePolynomial substitute(char var, F val) const {
        std::vector<char> variables = getVariables();
        if (std::find(variables.begin(), variables.end(), var) == variables.end()) {
            return MultivariatePolynomial<F>(*this);
        }

        std::map<Monomial, F> resultCoefficients;
        for (const auto& [monomial, coefficient] : _coefficients) {

            int currentExponent = monomial.getExponent(var);
            F newCoefficient = coefficient * _power(val, currentExponent);

            std::map<char, int> newMonomial = monomial.getMonomial();
            if (newMonomial.find(var) != newMonomial.end()) {
                newMonomial.erase(var);
            }

            resultCoefficients[Monomial(newMonomial)] += newCoefficient;
        }
        return MultivariatePolynomial(resultCoefficients);
    }

    /**
     * @brief Returns the leading monomial of the polynomial with respect to the given monomial
     * order
     */
    const Monomial& leadingMonomial(const MonomialOrder& order) const {
        _cacheLeadingMonomialAndCoefficient(order);
        return _cachedLeadingMonomial;
    }

    /**
     * @brief Returns the leading coefficient of the polynomial with respect to the given monomial
     * order
     */
    F leadingCoefficient(const MonomialOrder& order) const {
        _cacheLeadingMonomialAndCoefficient(order);
        return _cachedLeadingCoefficient;
    }

private:
    std::map<Monomial, F> _coefficients;
    mutable Monomial _cachedLeadingMonomial;
    mutable F _cachedLeadingCoefficient;
    mutable const MonomialOrder* _cachedOrder = nullptr;
    mutable bool _validLeadingTerm = false;

    F _power(F base, int exp) const {
        F result = F::one();
        while (exp > 0) {
            if (exp % 2 == 1) {
                result *= base;
            }
            base *= base;
            exp /= 2;
        }
        return result;
    }

    void _cacheLeadingMonomialAndCoefficient(const MonomialOrder& order) const {
        //  Only recompute if the order has changed or cache is invalid
        if (_cachedOrder != &order || !_validLeadingTerm) {
            auto maxIt = std::max_element(_coefficients.begin(), _coefficients.end(),
                                          [&order](const auto& p1, const auto& p2) {
                                              return order.compare(p1.first, p2.first);
                                          });

            if (maxIt == _coefficients.end()) {
                _cachedLeadingMonomial = Monomial();
                _cachedLeadingCoefficient = F::zero();
            }
            else {
                _cachedLeadingMonomial = maxIt->first;
                _cachedLeadingCoefficient = maxIt->second;
            }

            _cachedOrder = &order;
            _validLeadingTerm = true;
        }
    }
};

/**
 * @brief Returns a polynomial `var` over field `F`.
 */
template<typename F> MultivariatePolynomial<F> defineVariable(char var) {
    std::map<char, int> exponents;
    exponents[var] = 1;
    Monomial monomial(exponents);
    return MultivariatePolynomial<F>({
        {monomial, F::one()}
    });
}

#endif //  MULTIVARIATE_POLYNOMIAL_HPP