#include "GaloisField.hpp"
#include "GroebnerBasis.hpp"
#include "Monomial.hpp"
#include "MonomialOrders.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"
#include "Solver.hpp"
#include "UnivariatePolynomial.hpp"

#include <algorithm>
#include <emscripten/bind.h>

using namespace emscripten;

struct SystemResult__Rational {
    bool success = false;
    std::string error = "unknown error";
    std::vector<MultivariatePolynomial<Rational>> polynomials;
    std::vector<std::string> variables;
};

struct SystemResult__GaloisField {
    bool success = false;
    std::string error = "unknown error";
    std::vector<MultivariatePolynomial<GaloisField>> polynomials;
    std::vector<std::string> variables;
};

struct SystemResult__Real {
    bool success = false;
    std::string error = "unknown error";
    std::vector<MultivariatePolynomial<Real>> polynomials;
    std::vector<std::string> variables;
};

std::string
    printCharacteristicEquations__Rational(const std::vector<MultivariatePolynomial<Rational>>& X) {

    auto solution = characteristicEquations(X);
    if (solution.empty()) {
        return "No solutions found";
    }

    for (auto& [var, f] : solution) {
        int64_t lcm_denominators = 1;
        for (const auto& [monomial, coeff] : f.getCoefficients()) {
            lcm_denominators = std::lcm(lcm_denominators, coeff.getDenominator());
        }
        f *= lcm_denominators;
    }

    return printCharacteristicEquations(solution);
}

std::string printCharacteristicEquations__GaloisField(
    const std::vector<MultivariatePolynomial<GaloisField>>& X) {
    auto solution = characteristicEquations(X);
    return solution.empty() ? "No solutions found" : printCharacteristicEquations(solution);
}

std::string printCharacteristicEquations__Real(const std::vector<MultivariatePolynomial<Real>>& X) {
    auto solution = characteristicEquations(X);
    return solution.empty() ? "No solutions found" : printCharacteristicEquations(solution);
}

std::string printSystemSolution__Rational(const std::vector<MultivariatePolynomial<Rational>>& X) {
    auto solution = solveSystem<Rational>(X, findRationalRoots);
    return std::holds_alternative<std::string>(solution) ?
               std::get<std::string>(solution) :
               printSolutions(std::get<std::vector<std::map<char, Rational>>>(solution));
}

std::string
    printSystemSolution__GaloisField(const std::vector<MultivariatePolynomial<GaloisField>>& X) {
    auto solution = solveSystem<GaloisField>(X, findGaloisFieldRoots);
    return std::holds_alternative<std::string>(solution) ?
               std::get<std::string>(solution) :
               printSolutions(std::get<std::vector<std::map<char, GaloisField>>>(solution));
}

std::string printSystemSolution__Real(const std::vector<MultivariatePolynomial<Real>>& X) {
    auto solution = solveSystem<Real>(X, findRealRoots);
    return std::holds_alternative<std::string>(solution) ?
               std::get<std::string>(solution) :
               printSolutions(std::get<std::vector<std::map<char, Real>>>(solution));
}

SystemResult__Rational
    buildSystemFromStrings__Rational(const std::vector<std::string>& polyStrings) {
    SystemResult__Rational result;
    std::set<char> variableSet;

    for (int i = 0; i < polyStrings.size(); ++i) {
        try {
            MultivariatePolynomial<Rational> f = MultivariatePolynomial<Rational>(polyStrings[i]);

            if (f.isZeroPolynomial()) {
                continue;
            }

            result.polynomials.push_back(f);
            std::vector<char> f_vars = f.getVariables();
            variableSet.insert(f_vars.begin(), f_vars.end());
        }
        catch (const std::exception& e) {
            result.error = "Error parsing polynomial " + std::to_string(i + 1) + " (\"" +
                           polyStrings[i] + "\"): " + e.what();
            result.success = false;
            result.polynomials = {};
            result.variables = {};
            return result;
        }
    }

    result.variables.reserve(variableSet.size());
    for (char c : variableSet) {
        result.variables.push_back(std::string(1, c));
    }

    result.error = "No error!";
    result.success = true;
    return result;
}

SystemResult__GaloisField
    buildSystemFromStrings__GaloisField(const std::vector<std::string>& polyStrings) {
    SystemResult__GaloisField result;
    std::set<char> variableSet;

    for (int i = 0; i < polyStrings.size(); ++i) {
        try {
            MultivariatePolynomial<GaloisField> f =
                MultivariatePolynomial<GaloisField>(polyStrings[i]);

            if (f.isZeroPolynomial()) {
                continue;
            }

            result.polynomials.push_back(f);
            std::vector<char> f_vars = f.getVariables();
            variableSet.insert(f_vars.begin(), f_vars.end());
        }
        catch (const std::exception& e) {
            result.error = "Error parsing polynomial " + std::to_string(i + 1) + " (\"" +
                           polyStrings[i] + "\"): " + e.what();
            result.success = false;
            result.polynomials = {};
            result.variables = {};
            return result;
        }
    }

    result.variables.reserve(variableSet.size());
    for (char c : variableSet) {
        result.variables.push_back(std::string(1, c));
    }

    result.error = "No error!";
    result.success = true;
    return result;
}

SystemResult__Real buildSystemFromStrings__Real(const std::vector<std::string>& polyStrings) {
    SystemResult__Real result;
    std::set<char> variableSet;

    for (int i = 0; i < polyStrings.size(); ++i) {
        try {
            MultivariatePolynomial<Real> f = MultivariatePolynomial<Real>(polyStrings[i]);

            if (f.isZeroPolynomial()) {
                continue;
            }

            result.polynomials.push_back(f);
            std::vector<char> f_vars = f.getVariables();
            variableSet.insert(f_vars.begin(), f_vars.end());
        }
        catch (const std::exception& e) {
            result.error = "Error parsing polynomial " + std::to_string(i + 1) + " (\"" +
                           polyStrings[i] + "\"): " + e.what();
            result.success = false;
            result.polynomials = {};
            result.variables = {};
            return result;
        }
    }

    result.variables.reserve(variableSet.size());
    for (char c : variableSet) {
        result.variables.push_back(std::string(1, c));
    }

    result.error = "No error!";
    result.success = true;
    return result;
}

std::string inputStringToBetterString__Rational(const std::string& polyStr) {
    try {
        MultivariatePolynomial<Rational> f = MultivariatePolynomial<Rational>(polyStr);
        return f.toString();
    }
    catch (const std::exception& e) {
        return "Error parsing polynomial";
    }
}

std::string inputStringToBetterString__GaloisField(const std::string& polyStr) {
    try {
        MultivariatePolynomial<GaloisField> f = MultivariatePolynomial<GaloisField>(polyStr);
        return f.toString();
    }
    catch (const std::exception& e) {
        return "Error parsing polynomial";
    }
}

std::string inputStringToBetterString__Real(const std::string& polyStr) {
    try {
        MultivariatePolynomial<Real> f = MultivariatePolynomial<Real>(polyStr);
        return f.toString();
    }
    catch (const std::exception& e) {
        return "Error parsing polynomial";
    }
}

EMSCRIPTEN_BINDINGS(polynomial_solver) {
    value_object<SystemResult__Rational>("SystemResult__Rational")
        .field("success", &SystemResult__Rational::success)
        .field("error", &SystemResult__Rational::error)
        .field("polynomials", &SystemResult__Rational::polynomials)
        .field("variables", &SystemResult__Rational::variables);

    value_object<SystemResult__GaloisField>("SystemResult__GaloisField")
        .field("success", &SystemResult__GaloisField::success)
        .field("error", &SystemResult__GaloisField::error)
        .field("polynomials", &SystemResult__GaloisField::polynomials)
        .field("variables", &SystemResult__GaloisField::variables);

    value_object<SystemResult__Real>("SystemResult__Real")
        .field("success", &SystemResult__Real::success)
        .field("error", &SystemResult__Real::error)
        .field("polynomials", &SystemResult__Real::polynomials)
        .field("variables", &SystemResult__Real::variables);

    class_<MultivariatePolynomial<Rational>>("MultivariatePolynomial__Rational")
        .function("toString", &MultivariatePolynomial<Rational>::toString);

    class_<MultivariatePolynomial<GaloisField>>("MultivariatePolynomial__GaloisField")
        .function("toString", &MultivariatePolynomial<GaloisField>::toString);

    class_<MultivariatePolynomial<Real>>("MultivariatePolynomial__Real")
        .function("toString", &MultivariatePolynomial<Real>::toString);

    register_vector<MultivariatePolynomial<Rational>>("PolynomialVector__Rational");
    register_vector<MultivariatePolynomial<GaloisField>>("PolynomialVector__GaloisField");
    register_vector<MultivariatePolynomial<Real>>("PolynomialVector__Real");
    register_vector<std::string>("StringVector");

    class_<GaloisField>("GaloisField").class_function("setPrime", &GaloisField::setPrime);

    function("buildSystemFromStrings__Rational", &buildSystemFromStrings__Rational);
    function("buildSystemFromStrings__GaloisField", &buildSystemFromStrings__GaloisField);
    function("buildSystemFromStrings__Real", &buildSystemFromStrings__Real);

    function("printSystemSolution__Rational", &printSystemSolution__Rational);
    function("printSystemSolution__GaloisField", &printSystemSolution__GaloisField);
    function("printSystemSolution__Real", &printSystemSolution__Real);

    function("printCharacteristicEquations__Rational", &printCharacteristicEquations__Rational);
    function("printCharacteristicEquations__GaloisField",
             &printCharacteristicEquations__GaloisField);
    function("printCharacteristicEquations__Real", &printCharacteristicEquations__Real);

    function("inputStringToBetterString__Rational", &inputStringToBetterString__Rational);
    function("inputStringToBetterString__GaloisField", &inputStringToBetterString__GaloisField);
    function("inputStringToBetterString__Real", &inputStringToBetterString__Real);
}