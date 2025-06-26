#ifndef MONOMIAL_HPP
#define MONOMIAL_HPP

#include <algorithm>
#include <limits>
#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

/**
 * @brief Represents a monomial in a polynomial ring. It is stored as map where each variable has
 * its corresponding exponent.
 *
 */
class Monomial {

public:
    Monomial() : _degree(0), _numVariables(0) { }

    explicit Monomial(std::map<char, int> monomial) : _degree(0), _numVariables(0) {
        for (const auto& [var, exp] : monomial) {
            if (exp == 0) {
                continue; //  Skip terms with zero exponent
            }
            else if (exp < 0) {
                throw std::invalid_argument("Invalid exponent: " + std::to_string(exp) +
                                            " for variable '" + std::string(1, var) + "'");
            }
            _degree += exp;
            _numVariables++;
            _monomial[var] = exp;
        }
    }

    Monomial(const Monomial& other)
        : _monomial(other._monomial), _degree(other._degree), _numVariables(other._numVariables) { }

    explicit Monomial(const std::string& str) : _degree(0), _numVariables(0) {
        if (str.empty()) {
            _monomial = {};
            return;
        }

        auto it = str.begin();
        while (it != str.end()) {
            char var = *it;

            if (!((var >= 'A' && var <= 'Z') || (var >= 'a' && var <= 'z'))) {
                throw std::invalid_argument(
                    str + " cannot be converted into a Monomial instance [invalid variable]");
            }

            if (_monomial.count(var)) {
                throw std::invalid_argument(
                    str + " cannot be converted into a Monomial instance [duplicate variable: " +
                    var + "]");
            }

            ++it;

            if (it == str.end()) {
                _monomial[var] = 1;
                _degree++;
                _numVariables++;
                return;
            }

            char powerSign = *it;
            if (powerSign != '^' &&
                !((powerSign >= 'A' && var <= 'Z') || (powerSign >= 'a' && powerSign <= 'z'))) {
                throw std::invalid_argument(
                    str + " cannot be converted into a Monomial instance [invalid power sign]");
            }

            if (powerSign != '^') {
                _monomial[var] = 1;
                _degree++;
                _numVariables++;
                continue;
            }

            ++it;
            if (it == str.end()) {
                throw std::invalid_argument(
                    str +
                    " cannot be converted into a Monomial instance [no exponent given after ^]");
            }

            std::string expStr;
            while (it != str.end() && std::isdigit(*it)) {
                expStr += *it;
                ++it;
            }

            if (expStr.empty()) {
                throw std::invalid_argument(
                    str + " cannot be converted into a Monomial instance [empty exponent]");
            }

            int exp = std::stoi(expStr);
            if (exp <= 0) {
                throw std::invalid_argument(
                    str + " cannot be converted into a Monomial instance [negative]");
            }

            _monomial[var] = exp;
            _degree += exp;
            _numVariables++;
        }
    }

    Monomial& operator=(const Monomial& other) {
        if (this != &other) {
            _monomial = other._monomial;
            _degree = other._degree;
            _numVariables = other._numVariables;
        }
        return *this;
    }

    int getDegree() const {
        return _degree;
    }

    int getNumVariables() const {
        return _numVariables;
    }

    std::map<char, int> getMonomial() const {
        return _monomial;
    }

    std::vector<char> getVariables() const {
        std::vector<char> variables;
        for (const auto& [var, exp] : _monomial) {
            variables.push_back(var);
        }
        return variables;
    }

    int getExponent(char var) const {
        if (_monomial.find(var) == _monomial.end()) {
            return 0;
        }
        return _monomial.at(var);
    }

    bool operator==(const Monomial& other) const {
        return _monomial == other._monomial;
    }

    bool operator!=(const Monomial& other) const {
        return _monomial != other._monomial;
    }

    bool operator<(const Monomial& other) const {
        if (_degree != other.getDegree()) {
            return _degree < other.getDegree();
        }

        auto itLeft = _monomial.begin();
        auto itRight = other._monomial.begin();

        while (itLeft != _monomial.end() && itRight != other._monomial.end()) {

            const auto& [varLeft, expLeft] = *itLeft;
            const auto& [varRight, expRight] = *itRight;

            if (varLeft == varRight) {
                if (expLeft < expRight) {
                    return true;
                }
                else if (expLeft > expRight) {
                    return false;
                }
                itLeft++;
                itRight++;
            }
            else {
                return varLeft > varRight;
            }
        }

        if (itLeft == _monomial.end() && itRight == other._monomial.end()) {
            return false;
        }
        else if (itLeft == _monomial.end()) {
            return true;
        }
        else {
            return false;
        }
    }

    bool operator>(const Monomial& other) const {
        return other < *this;
    }

    bool operator<=(const Monomial& other) const {
        return !(*this > other);
    }

    bool operator>=(const Monomial& other) const {
        return !(*this < other);
    }

    Monomial operator*(const Monomial& other) const {
        std::map<char, int> result = _monomial;
        for (const auto& [var, exp] : other._monomial) {
            result[var] += exp;
        }
        return Monomial(result);
    }

    Monomial operator*=(const Monomial& other) {
        for (const auto& [var, exp] : other._monomial) {
            _monomial[var] += exp;
        }
        return *this;
    }

    Monomial operator/(const Monomial& other) const {
        std::map<char, int> result = _monomial;
        for (const auto& [var, exp] : other._monomial) {
            result[var] -= exp;
            if (result[var] < 0) {
                return Monomial::null;
            }
            else if (result[var] == 0) {
                result.erase(var);
            }
        }
        return Monomial(result);
    }

    Monomial operator/=(const Monomial& other) {
        for (const auto& [var, exp] : other._monomial) {
            _monomial[var] -= exp;
            if (_monomial[var] < 0) {
                return Monomial::null;
            }
            else if (_monomial[var] == 0) {
                _monomial.erase(var);
            }
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Monomial& monomial) {
        if (monomial.getDegree() == 0) {
            os << "1"; //  Empty monomial is equivalent to "1"
            return os;
        }

        std::ostringstream result;
        for (const auto& [var, exp] : monomial._monomial) {
            result << var << (exp == 1 ? "" : monomial._toSuperscript(exp));
        }

        os << result.str();
        return os;
    }

    std::string toString() const {
        std::ostringstream result;
        if (_degree == 0) {
            return "1"; //  Empty monomial is equivalent to "1"
        }

        for (const auto& [var, exp] : _monomial) {
            result << var << (exp == 1 ? "" : _toSuperscript(exp));
        }

        return result.str();
    }

    /**
     * Returns true iff `a` is divisible by `b` so that `a / b` wouldn't throw an exception
     */
     inline static bool divides(const Monomial& a, const Monomial& b) {
        for (const auto& [var, exp] : b.getMonomial()) {
            if (a.getExponent(var) - exp < 0) {
                return false;
            }
        }
        return true;
    }

    /**
     * Returns the least common multiple of two monomials, that is `lcm(a, b) = [max{a[i],
     * b[i]}]_i`
     */
    static Monomial lcm(const Monomial& a, const Monomial& b) {
        std::map<char, int> result = a._monomial;
        for (const auto& [var, exp] : b._monomial) {
            result[var] = std::max(result[var], exp);
        }
        return Monomial(result);
    }

    static Monomial null;


private:
    std::map<char, int> _monomial;
    int _degree;
    int _numVariables;

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


#endif //  MONOMIAL_HPP
