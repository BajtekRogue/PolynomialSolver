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
 * @brief Monomial is stored as a map where each variable has its corresponding
 * exponent.
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
            else {
                _degree += exp;
                _numVariables++;
                _monomial[var] = exp;
            }
        }
    }

    Monomial(const Monomial& other)
        : _monomial(other._monomial), _degree(other._degree), _numVariables(other._numVariables) { }

    explicit Monomial(const std::string& str) : _degree(0), _numVariables(0) {
        if (str.empty()) {
            return;
        }

        auto it = str.begin();
        while (it != str.end()) {
            char var = *it;

            if (!std::isalpha(static_cast<unsigned char>(var))) {
                throw std::invalid_argument("Invalid variable");
            }

            if (_monomial.count(var)) {
                throw std::invalid_argument("Duplicate variable: " + std::string(1, var));
            }

            ++it;

            if (it == str.end()) {
                _monomial[var] = 1;
                _degree++;
                _numVariables++;
                return;
            }

            char powerSign = *it;
            if (powerSign != '^' && !std::isalpha(static_cast<unsigned char>(powerSign))) {
                throw std::invalid_argument("Invalid variable");
            }

            if (powerSign != '^') {
                _monomial[var] = 1;
                _degree++;
                _numVariables++;
                continue;
            }

            ++it;
            if (it == str.end()) {
                throw std::invalid_argument("No exponent given after ^");
            }

            std::string expStr;
            while (it != str.end() && std::isdigit(*it)) {
                expStr += *it;
                ++it;
            }

            if (expStr.empty()) {
                throw std::invalid_argument("Empty exponent");
            }

            int exp = std::stoi(expStr);
            if (exp <= 0) {
                throw std::invalid_argument("Negative exponent");
            }

            _monomial[var] = exp;
            _degree += exp;
            _numVariables++;
        }
    }

    Monomial& operator=(const Monomial& other) = default;

    int getDegree() const {
        return _degree;
    }

    int getNumVariables() const {
        return _numVariables;
    }

    const std::map<char, int>& getMonomial() const {
        return _monomial;
    }

    std::vector<char> getVariables() const {
        std::vector<char> result;
        for (const auto& [var, exp] : _monomial) {
            result.push_back(var);
        }
        return result;
    }

    int getExponent(char var) const {
        auto it = _monomial.find(var);
        return (it != _monomial.end()) ? it->second : 0;
    }

    bool operator==(const Monomial& other) const {
        return _monomial == other._monomial;
    }

    bool operator!=(const Monomial& other) const {
        return _monomial != other._monomial;
    }

    bool operator<(const Monomial& other) const {
        if (_degree != other._degree) {
            return _degree < other._degree;
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

    Monomial& operator*=(const Monomial& other) {
        for (const auto& [var, exp] : other._monomial) {
            _monomial[var] += exp;
            _degree += exp;
        }
        _numVariables = _monomial.size();
        return *this;
    }

    Monomial operator/(const Monomial& other) const {
        std::map<char, int> result = _monomial;
        for (const auto& [var, exp] : other._monomial) {

            auto it = result.find(var);
            if (it == result.end() || it->second < exp) {
                return Monomial::null;
            }

            it->second -= exp;
            if (it->second == 0) {
                result.erase(it);
            }
        }

        return Monomial(result);
    }

    Monomial& operator/=(const Monomial& other) {
        for (const auto& [var, exp] : other._monomial) {

            auto it = _monomial.find(var);
            if (it == _monomial.end() || it->second < exp) {
                *this = Monomial::null;
                return *this;
            }

            it->second -= exp;
            _degree -= exp;
            if (it->second == 0) {
                _monomial.erase(it);
                _numVariables--;
            }
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Monomial& monomial) {
        if (monomial._degree == 0) {
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
     * Returns true iff `a` is divisible by `b` so that `a / b` is still a monomial
     */
    static bool divides(const Monomial& a, const Monomial& b) {
        for (const auto& [var, exp] : b.getMonomial()) {
            if (a.getExponent(var) < exp) {
                return false;
            }
        }
        return true;
    }

    /**
     * `lcm(a, b) = [max{a[i], b[i]}]_i`
     */
    static Monomial lcm(const Monomial& a, const Monomial& b) {
        std::map<char, int> result;

        //  Start with the larger monomial to minimize insertions
        const std::map<char, int>& larger =
            (a._numVariables >= b._numVariables) ? a._monomial : b._monomial;
        const std::map<char, int>& smaller =
            (a._numVariables < b._numVariables) ? a._monomial : b._monomial;
        result = larger;

        for (const auto& [var, exp] : smaller) {
            auto [it, inserted] = result.try_emplace(var, exp);
            if (!inserted) {
                it->second = std::max(it->second, exp);
            }
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
