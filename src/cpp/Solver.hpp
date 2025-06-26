#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "GaloisField.hpp"
#include "GroebnerBasis.hpp"
#include "Monomial.hpp"
#include "MonomialOrders.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"
#include "Real.hpp"
#include "UnivariatePolynomial.hpp"

#include <functional>
#include <variant>

std::vector<Rational> findRationalRoots(const UnivariatePolynomial<Rational>& f);
std::vector<GaloisField> findGaloisFieldRoots(const UnivariatePolynomial<GaloisField>& f);
std::vector<Real> findRealRoots(const UnivariatePolynomial<Real>& f);

template<typename F>
UnivariatePolynomial<F> fromMultivariateToUnivariate(const MultivariatePolynomial<F>& f) {
    std::vector<char> f_vars = f.getVariables();

    if (f_vars.size() != 1) {
        throw std::runtime_error(f.toString() + " is not univariate");
    }

    char var = f_vars.back();
    int degree = f.totalDegree();

    std::vector<F> result(degree + 1);

    for (auto& [monomial, coefficient] : f.getCoefficients()) {
        int power = monomial.getExponent(var);
        result[power] = coefficient;
    }

    return UnivariatePolynomial(result);
}

/**
 * @brief For a system of polynomial equations `X`, returns the characteristic equations that each
 * variable must satisfy. If the system has no solutions returns the empty map. Requires many
 * calculations of Groebner basis.
 */
template<typename F>
std::map<char, MultivariatePolynomial<F>>
    characteristicEquations(const std::vector<MultivariatePolynomial<F>>& X) {

    std::map<char, MultivariatePolynomial<F>> result;
    std::set<char> varSet;

    for (const MultivariatePolynomial<F>& f : X) {
        std::vector<char> f_vars = f.getVariables();
        varSet.insert(f_vars.begin(), f_vars.end());
    }

    //  Edge case
    if (X.size() > 1 && varSet.size() == 1) {
        return {};
    }

    for (char var : varSet) {
        std::vector<char> newPermutation;
        std::set<char> newVarSet = varSet;

        newVarSet.erase(var);
        newPermutation.insert(newPermutation.end(), newVarSet.begin(), newVarSet.end());
        newPermutation.push_back(var);

        std::vector<MultivariatePolynomial<F>> G =
            calculateGroebnerBasis(X, LexOrder(newPermutation));
        std::vector<MultivariatePolynomial<F>> H;

        for (const MultivariatePolynomial<F>& g : G) {
            std::vector<char> g_vars = g.getVariables();
            if (g_vars.size() == 1 && g_vars.back() == var) {
                H.push_back(g);
            }
        }

        if (H.size() != 1) {
            return {};
        }
        else {
            result[var] = H.front();
        }
    }

    return result;
}

/**
 * @brief Helper function to solve system of polynomial equations
 */
template<typename F>
std::variant<std::string, std::vector<std::map<char, F>>>
    recursiveSolver(const std::vector<MultivariatePolynomial<F>>& X,
                    std::function<std::vector<F>(const UnivariatePolynomial<F>&)> rootFinder) {

    if (X.empty()) {
        return {};
    }

    std::vector<MultivariatePolynomial<F>> univaratePolynomials;
    std::vector<MultivariatePolynomial<F>> constantPolynomials;

    for (const MultivariatePolynomial<F>& f : X) {
        std::vector<char> f_vars = f.getVariables();
        if (f_vars.size() == 1) {
            univaratePolynomials.push_back(f);
        }
        else if (f_vars.size() == 0 && !f.isZeroPolynomial()) {
            constantPolynomials.push_back(f);
        }
    }

    if (!constantPolynomials.empty()) {
        return "No solutions found";
    }
    else if (univaratePolynomials.empty()) {
        return "There are infinitely many solutions";
    }

    MultivariatePolynomial<F> f = univaratePolynomials.front();
    char var = f.getVariables().front();
    univaratePolynomials.erase(univaratePolynomials.begin());
    std::vector<F> rootsFound = rootFinder(fromMultivariateToUnivariate(f));
    std::cout << "roots for f = " << f << "\n";
    if (rootsFound.empty()) {
        return "No solutions found";
    }

    std::vector<std::map<char, F>> solutions;

    for (F root : rootsFound) {
        std::map<char, F> currentSolution = {
            {var, root}
        };
        std::vector<MultivariatePolynomial<F>> G;
        std::cout << "root = " << root << "\n";
        for (const MultivariatePolynomial<F>& f : X) {
            MultivariatePolynomial<F> g = f.substitute(var, root);
            if (!g.isZeroPolynomial()) {
                G.push_back(g);
            }
        }


        if (G.empty()) {
            solutions.push_back(currentSolution);
        }
        else {
            std::variant<std::string, std::vector<std::map<char, F>>> extendedSolution =
                recursiveSolver(G, rootFinder);

            if (std::holds_alternative<std::string>(extendedSolution)) {
                return std::get<std::string>(extendedSolution);
            }

            std::vector<std::map<char, F>> extendedSolutions =
                std::get<std::vector<std::map<char, F>>>(extendedSolution);

            for (const std::map<char, F>& sol : extendedSolutions) {
                std::map<char, F> fullSolution = currentSolution;
                fullSolution.insert(sol.begin(), sol.end());
                solutions.push_back(fullSolution);
            }
        }
    }

    return solutions;
}

/**
 * @brief Solve system of polynomial equations. If there are no solutions in the field returns the
 * empty vector and if there are infinitly many solutions or no solutions in any field extension a
 * `runtime_error` is thrown
 */
template<typename F>
std::variant<std::string, std::vector<std::map<char, F>>>
    solveSystem(const std::vector<MultivariatePolynomial<F>>& X,
                std::function<std::vector<F>(const UnivariatePolynomial<F>&)> rootFinder) {

    if (X.empty()) {
        return "Empty system is not allowed";
    }

    std::set<char> varSet;
    for (const MultivariatePolynomial<F>& f : X) {
        std::vector<char> f_vars = f.getVariables();
        varSet.insert(f_vars.begin(), f_vars.end());
    }
    std::vector<char> variables(varSet.begin(), varSet.end());

    //  Hilbert Nullstellensatz
    std::vector<MultivariatePolynomial<F>> G = calculateGroebnerBasis(X, LexOrder(variables));
    if (G.size() == 1 && G.front() == F::one()) {
        return "No solution exist in any field extension";
    }
    return recursiveSolver(G, rootFinder);
}

template<typename F>
std::string printCharacteristicEquations(const std::map<char, MultivariatePolynomial<F>>& X) {

    std::string result;
    for (auto& [var, p] : X) {
        result += std::string(1, var) + ": " + p.toString() + "\n";
    }
    return result;
}

template<typename F> std::string printSolutions(const std::vector<std::map<char, F>>& X) {

    std::string result;
    for (int i = 0; i < X.size(); i++) {
        result += "Solution " + std::to_string(i + 1) + ":\n";

        for (auto& [var, val] : X[i]) {
            result += "   " + std::string(1, var) + " = " + val.toString() + "\n";
        }
        result += "\n\n";
    }

    return result;
}

#endif //  SOLVER_HPP