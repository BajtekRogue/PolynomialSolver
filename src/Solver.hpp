#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "BigRational.hpp"
#include "GaloisField.hpp"
#include "GroebnerBasis.hpp"
#include "Logger.hpp"
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
std::vector<BigRational> findBigRationalRoots(const UnivariatePolynomial<BigRational>& f);

template<typename F>
UnivariatePolynomial<F> fromMultivariateToUnivariate(const MultivariatePolynomial<F>& f) {
    std::vector<char> f_vars = f.getVariables();

    if (f_vars.size() != 1) {
        throw std::runtime_error(f.toString() + " is not univariate");
    }

    char var = f_vars.front();
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
 * variable must satisfy. If the system has no solutions returns the empty map. Requires `|X|`
 * calculations of Groebner basis.
 */
template<typename F>
std::map<char, MultivariatePolynomial<F>>
    characteristicEquations(const std::vector<MultivariatePolynomial<F>>& X) {

    Logger::characteristicEq("🎯 === characteristicEquations CALLED ===");
    Logger::characteristicEq("📊 System size: " + std::to_string(X.size()));

    std::map<char, MultivariatePolynomial<F>> result;
    std::set<char> varSet;

    for (const MultivariatePolynomial<F>& f : X) {
        std::vector<char> f_vars = f.getVariables();
        varSet.insert(f_vars.begin(), f_vars.end());
    }

    std::string varSetStr = "🔤 All variables: ";
    for (char v : varSet) {
        varSetStr += v;
        varSetStr += " ";
    }
    Logger::characteristicEq(varSetStr);

    //  Edge case
    if (X.size() > 1 && varSet.size() == 1) {
        Logger::characteristicEq(
            "⚠️ Edge case: Multiple polynomials with single variable - no characteristic equations");
        return {};
    }

    for (char var : varSet) {
        Logger::characteristicEq("🎪 Computing characteristic equation for variable: " +
                                 std::string(1, var));

        std::vector<char> newPermutation;
        std::set<char> newVarSet = varSet;

        newVarSet.erase(var);
        newPermutation.insert(newPermutation.end(), newVarSet.begin(), newVarSet.end());
        newPermutation.push_back(var);

        std::string permStr = "🔀 Variable order: ";
        for (char c : newPermutation) {
            permStr += c;
            permStr += " ";
        }
        Logger::characteristicEq(permStr);

        Logger::characteristicEq("⚙️ Calculating Groebner basis...");
        std::vector<MultivariatePolynomial<F>> G =
            calculateGroebnerBasis(X, LexOrder(newPermutation));
        Logger::characteristicEq("✨ Groebner basis computed, size: " + std::to_string(G.size()));

        std::vector<MultivariatePolynomial<F>> H;

        for (const MultivariatePolynomial<F>& g : G) {
            std::vector<char> g_vars = g.getVariables();
            Logger::characteristicEq("🔎 Examining basis element: " + g.toString());

            if (g_vars.size() == 1 && g_vars.front() == var) {
                Logger::characteristicEq("🎯 Found univariate polynomial in " +
                                         std::string(1, var));
                H.push_back(g);
            }
        }

        if (H.size() != 1) {
            Logger::characteristicEq("❌ Expected exactly 1 univariate polynomial, found: " +
                                     std::to_string(H.size()));
            return {};
        }

        result[var] = H.front();
        Logger::characteristicEq("✅ Characteristic equation for " + std::string(1, var) + ": " +
                                 H.front().toString());
    }

    Logger::characteristicEq("🎉 All characteristic equations computed successfully");
    return result;
}

/**
 * @brief Helper function to solve system of polynomial equations
 */
template<typename F>
std::variant<std::string, std::vector<std::map<char, F>>>
    recursiveSolver(const std::vector<MultivariatePolynomial<F>>& X,
                    std::function<std::vector<F>(const UnivariatePolynomial<F>&)> rootFinder) {
    Logger::solver("🌀 === recursiveSolver CALLED ===");
    Logger::solver("📦 Input size: " + std::to_string(X.size()));

    for (size_t i = 0; i < X.size(); ++i) {
        Logger::solver("🔢 X[" + std::to_string(i) + "] = " + X[i].toString());
    }

    if (X.empty()) {
        Logger::solver("⚠️ Empty system. Returning empty solution.");
        return {};
    }

    std::vector<MultivariatePolynomial<F>> univaratePolynomials;
    std::vector<MultivariatePolynomial<F>> constantPolynomials;

    for (const MultivariatePolynomial<F>& f : X) {
        std::vector<char> f_vars = f.getVariables();

        if (f_vars.size() == 1) {
            univaratePolynomials.push_back(f);
        }
        else if (f_vars.empty() && !f.isZeroPolynomial()) {
            constantPolynomials.push_back(f);
        }
    }

    if (!constantPolynomials.empty()) {
        Logger::solver("❌ Returning: No solutions found");
        return "No solutions found";
    }
    else if (univaratePolynomials.empty()) {
        Logger::solver("♾️ No univariate polynomials left. Returning: Infinitely many solutions");
        return "There are infinitely many solutions";
    }

    MultivariatePolynomial<F> f = univaratePolynomials.front();
    char var = f.getVariables().front();
    Logger::solver("🎯 Selected univariate polynomial f(" + std::string(1, var) +
                   ") = " + f.toString());
    Logger::solver("📌 Variable selected: " + std::string(1, var));

    univaratePolynomials.erase(univaratePolynomials.begin());
    std::vector<F> rootsFound = rootFinder(fromMultivariateToUnivariate(f));

    std::string rootsStr = "   🌱 Roots found: ";
    for (const F& r : rootsFound) {
        rootsStr += r.toString() + " ";
    }
    Logger::solver(rootsStr);

    if (rootsFound.empty()) {
        Logger::solver("   ❌ No roots found. Returning: No solutions found");
        return "No solutions found";
    }

    std::vector<std::map<char, F>> solutions;

    for (const F& root : rootsFound) {

        std::map<char, F> currentSolution = {
            {var, root}
        };
        std::vector<MultivariatePolynomial<F>> G;

        Logger::solver("   🧪 Trying root: " + root.toString() + " for variable " + var);
        Logger::solver("   📉 Substituting " + std::string(1, var) + " = " + root.toString() +
                       " into all polynomials...");

        for (const MultivariatePolynomial<F>& f : X) {

            MultivariatePolynomial<F> g = f.substitute(var, root);

            if (!g.isZeroPolynomial()) {
                G.push_back(g);
                Logger::solver("🔁 " + f.toString() + " → " + g.toString());
            }
            else {
                Logger::solver("🐼 " + f.toString() + " → " + g.toString() + "");
            }
        }

        if (G.empty()) {
            Logger::solver(
                "   ✅ All polynomials vanished after substitution. Partial solution accepted.");
            solutions.push_back(currentSolution);
        }
        else {
            Logger::solver("   🔁 Recursively solving remaining system of size " +
                           std::to_string(G.size()));
            auto extendedSolution = recursiveSolver(G, rootFinder);

            if (std::holds_alternative<std::string>(extendedSolution)) {
                std::string message = std::get<std::string>(extendedSolution);
                Logger::solver("   ⚠️ Recursive call returned a string: " + message);

                if (message == "There are infinitely many solutions") {
                    Logger::solver("   😡 Infinitely many solutions, system will not be solved");
                    return "There are infinitely many solutions";
                }
                else {
                    Logger::solver("   🔥 No solution found for this root = " + root.toString() +
                                   " ,  skiping to the next");
                    continue;
                }
            }

            std::vector<std::map<char, F>> extendedSolutions =
                std::get<std::vector<std::map<char, F>>>(extendedSolution);

            for (const std::map<char, F>& sol : extendedSolutions) {
                std::map<char, F> fullSolution = currentSolution;
                fullSolution.insert(sol.begin(), sol.end());

                std::string solStr = "   🔗 Merged solution: ";
                for (const auto& [k, v] : fullSolution) {
                    solStr += std::string(1, k) + " = " + v.toString() + ", ";
                }
                Logger::solver(solStr);
                solutions.push_back(fullSolution);
            }
        }
    }

    Logger::solver("🧾 Returning " + std::to_string(solutions.size()) + " solution(s)");
    return solutions;
}

/**
 * @brief Solves system of polynomial equations. If there are solutions returns vector of them.
 * Otherwise string with coressponding message is returned.
 */
template<typename F>
std::variant<std::string, std::vector<std::map<char, F>>>
    solveSystem(const std::vector<MultivariatePolynomial<F>>& X,
                std::function<std::vector<F>(const UnivariatePolynomial<F>&)> rootFinder) {

    Logger::solver("🚀 === solveSystem CALLED ===");
    Logger::solver("📥 System size: " + std::to_string(X.size()));

    if (X.empty()) {
        Logger::solver("❌ Empty system not allowed");
        return "Empty system is not allowed";
    }

    std::set<char> varSet;
    for (const MultivariatePolynomial<F>& f : X) {
        std::vector<char> f_vars = f.getVariables();
        varSet.insert(f_vars.begin(), f_vars.end());
        Logger::solver("📝 Input polynomial: " + f.toString());
    }

    std::vector<char> variables(varSet.begin(), varSet.end());
    std::string varStr = "🎲 Variables in system: ";
    for (char v : variables) {
        varStr += v;
        varStr += " ";
    }
    Logger::solver(varStr);

    //  Hilbert Nullstellensatz
    Logger::solver("⚙️ Computing Groebner basis for Nullstellensatz check...");
    std::vector<MultivariatePolynomial<F>> G = calculateGroebnerBasis(X, LexOrder(variables));
    Logger::solver("✨ Groebner basis computed, size: " + std::to_string(G.size()));

    if (G.size() == 1 && G.front() == F::one) {
        Logger::solver("🚫 Nullstellensatz: System has no solutions in any field extension");
        return "No solution exist in any field extension";
    }

    Logger::solver("🔄 Proceeding to recursive solver...");
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