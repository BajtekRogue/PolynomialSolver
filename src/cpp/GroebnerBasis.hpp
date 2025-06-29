#ifndef GROEBNER_BASIS_HPP
#define GROEBNER_BASIS_HPP

#include "Logger.hpp"
#include "Monomial.hpp"
#include "MonomialOrders.hpp"
#include "MultivariatePolynomial.hpp"

/**
 * @brief Division algorithm for multivariable polynomials. Size of quotient vector is equal to the
 * size of the divisor vector. In general Result depends on the order of elements in
 * `G` as well as the monomial order choosen.
 */
template<typename F>
std::pair<std::vector<MultivariatePolynomial<F>>, MultivariatePolynomial<F>>
    polynomialReduce(const MultivariatePolynomial<F>& f,
                     const std::vector<MultivariatePolynomial<F>>& G, const MonomialOrder& order) {

    const int n = G.size();
    MultivariatePolynomial<F> p(f);
    MultivariatePolynomial<F> r;
    std::vector<MultivariatePolynomial<F>> Q(n);

    //  Reduction process
    while (!p.isZeroPolynomial()) {

        const Monomial& p_leadingMonomial = p.leadingMonomial(order);
        const F p_leadingCoefficient = p.leadingCoefficient(order);
        bool somethingDivided = false;

        for (int i = 0; i < n; i++) {

            const MultivariatePolynomial<F>& g = G[i];
            const Monomial& g_leadingMonomial = g.leadingMonomial(order);
            F g_leadingCoefficient = g.leadingCoefficient(order);

            //  Check if something divides the leading term of p
            Monomial divisionMonomial = p_leadingMonomial / g_leadingMonomial;
            if (divisionMonomial == Monomial::null) {
                continue; //  Didn't divide
            }

            F divisionCoefficient = p_leadingCoefficient / g_leadingCoefficient;
            MultivariatePolynomial<F> divisionMonomialPolynomial({
                {divisionMonomial, divisionCoefficient}
            });

            p -= divisionMonomialPolynomial * g;
            Q[i] += divisionMonomialPolynomial;
            somethingDivided = true;

            break;
        }

        //  Nothing divided so reduce p and update r
        if (!somethingDivided) {
            MultivariatePolynomial<F> divisionMonomialPolynomial({
                {p_leadingMonomial, p_leadingCoefficient}
            });
            p -= divisionMonomialPolynomial;
            r += divisionMonomialPolynomial;
        }
    }

    return {Q, r};
}

/**
 * `S(f, g) = lcm(LM(f), LM(g)) * (f / LT(f)  - g / LT(g))`
 */
template<typename F>
MultivariatePolynomial<F> syzygy(const MultivariatePolynomial<F>& f,
                                 const MultivariatePolynomial<F>& g, const MonomialOrder& order) {

    const Monomial& f_leadingMonomial = f.leadingMonomial(order);
    F f_leadingCoefficient = f.leadingCoefficient(order);

    const Monomial& g_leadingMonomial = g.leadingMonomial(order);
    F g_leadingCoefficient = g.leadingCoefficient(order);

    Monomial lcm = Monomial::lcm(f_leadingMonomial, g_leadingMonomial);
    MultivariatePolynomial<F> u({
        {lcm / f_leadingMonomial, F::one / f_leadingCoefficient}
    });
    MultivariatePolynomial<F> v({
        {lcm / g_leadingMonomial, F::one / g_leadingCoefficient}
    });

    return u * f - v * g;
}

template<typename F>
bool chainCriterion(const Monomial& lcm_ab, const std::vector<MultivariatePolynomial<F>>& G,
                    int startIdx, const MonomialOrder& order) {
    for (int k = startIdx; k < G.size(); k++) {
        if (Monomial::divides(lcm_ab, G[k].leadingMonomial(order))) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Extends set `X` to a Groebner basis using Buchberger's algorithm
 */
template<typename F>
std::vector<MultivariatePolynomial<F>>
    extendToGroebnerBasis(const std::vector<MultivariatePolynomial<F>>& X,
                          const MonomialOrder& order) {

    std::vector<MultivariatePolynomial<F>> G = X;
    Logger::groebnerBasis("📥 Initial basis size: " + std::to_string(X.size()));
    int iterationCount = 0;

    while (true) {
        iterationCount++;
        const int n = G.size();
        std::vector<MultivariatePolynomial<F>> H = G;
        bool somethingAdded = false;

        //  Statistics for this iteration
        const int totalPairs = n * (n - 1) / 2;
        int currentPair = 0;
        int lcmSkipped = 0;
        int chainSkipped = 0;
        int divisionsPerformed = 0;
        int newPolynomials = 0;

        Logger::groebnerBasis("🔄 ITERATION #" + std::to_string(iterationCount));
        Logger::groebnerBasis("   📊 Current basis size: " + std::to_string(n));
        Logger::groebnerBasis("   🧪 Pairs to check: " + std::to_string(totalPairs));
        Logger::printProgressBar(0, totalPairs);

        //  Iterate over all pairs (i, j) in G
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                currentPair++;

                const Monomial& i_monomial = G[i].leadingMonomial(order);
                const Monomial& j_monomial = G[j].leadingMonomial(order);
                Monomial lcm_ij = Monomial::lcm(i_monomial, j_monomial);

                //  First check lcmCriterion. If `a, b ` are relativly prime, we don't need to
                //  comput syzygy
                if (lcm_ij == i_monomial * j_monomial) {
                    lcmSkipped++;
                    Logger::printProgressBar(currentPair, totalPairs);
                    continue;
                }

                //  Second check chainCriterion. That occurs when there
                //  is a third monomial that divides the lcm of the two
                //  monomials
                if (chainCriterion(lcm_ij, G, j + 1, order)) {
                    chainSkipped++;
                    Logger::printProgressBar(currentPair, totalPairs);
                    continue;
                }

                //  Need to do division. If r is not 0, add it to H
                MultivariatePolynomial<F> s = syzygy(G[i], G[j], order);
                auto [_, r] = polynomialReduce(s, G, order);

                if (!r.isZeroPolynomial()) {
                    newPolynomials++;
                    H.push_back(r);
                    somethingAdded = true;
                }

                Logger::printProgressBar(currentPair, totalPairs);
            }
        }

        Logger::clearProgressBar();

        Logger::groebnerBasis("📈 ITERATION #" + std::to_string(iterationCount) + " STATISTICS:");
        Logger::groebnerBasis("   🚫 LCM criterion skipped: " + std::to_string(lcmSkipped) +
                              " pairs");
        Logger::groebnerBasis("   ⛓️  Chain criterion skipped: " + std::to_string(chainSkipped) +
                              " pairs");
        Logger::groebnerBasis("   ➗ Divisions performed: " + std::to_string(divisionsPerformed) +
                              " pairs");
        Logger::groebnerBasis("   ➕ New polynomials added: " + std::to_string(newPolynomials));

        if (totalPairs > 0) {
            double skipPercentage = 100.0 * (lcmSkipped + chainSkipped) / totalPairs;
            Logger::groebnerBasis(
                "   📊 Total skip rate: " + std::to_string(static_cast<int>(skipPercentage)) + "%");
        }


        //  If something was added, update G and continue, otherwise return
        if (!somethingAdded) {
            Logger::groebnerBasis("🎉 Groebner basis is complete!");
            Logger::groebnerBasis("📊 Final basis size: " + std::to_string(H.size()));
            return H;
        }

        Logger::groebnerBasis("🐨 Not yet a Groebner basis, continuing iteration...");
        G = std::move(H);
    }
}

/**
 * @brief Reduces a Groebner basis
 */
template<typename F>
std::vector<MultivariatePolynomial<F>>
    reduceGroebnerBasis(const std::vector<MultivariatePolynomial<F>>& G, const MonomialOrder& order,
                        bool normalizedCoefficients) {

    std::vector<MultivariatePolynomial<F>> H;
    H.reserve(G.size());

    //  First pass: Remove polynomials whose leading terms are divisible by others
    for (int i = 0; i < G.size(); i++) {
        const MultivariatePolynomial<F>& g = G[i];
        const Monomial& g_leadingMonomial = g.leadingMonomial(order);

        bool inLeadingTermsIdeal = false;

        //  Check against polynomials we've already kept
        for (const MultivariatePolynomial<F>& h : H) {
            if (Monomial::divides(g_leadingMonomial, h.leadingMonomial(order))) {
                inLeadingTermsIdeal = true;
                break;
            }
        }

        //  Check against remaining polynomials in G
        if (!inLeadingTermsIdeal) {
            for (int j = i + 1; j < G.size(); j++) {
                if (Monomial::divides(g_leadingMonomial, G[j].leadingMonomial(order))) {
                    inLeadingTermsIdeal = true;
                    break;
                }
            }
        }

        if (!inLeadingTermsIdeal) {
            H.push_back(g);
        }
    }

    //  Second pass: Perform reductions
    bool somethingReduced = true;
    while (somethingReduced) {
        somethingReduced = false;

        for (int i = 0; i < H.size(); i++) {
            //  Create divisor set without H[i] - use indices to avoid copying
            std::vector<int> divisorIndices;
            divisorIndices.reserve(H.size() - 1);
            for (int j = 0; j < H.size(); j++) {
                if (j != i) {
                    divisorIndices.push_back(j);
                }
            }

            //  Create temporary vector of divisors (unavoidable for polynomialReduce interface)
            std::vector<MultivariatePolynomial<F>> divisors;
            divisors.reserve(divisorIndices.size());
            for (int idx : divisorIndices) {
                divisors.push_back(H[idx]);
            }

            auto [_, r] = polynomialReduce(H[i], divisors, order);

            if (!r.isZeroPolynomial() && H[i] != r) {
                H[i] = std::move(r);
                somethingReduced = true;
            }
        }
    }

    //  Third [optional] pass: Normalize coefficients
    if (normalizedCoefficients) {
        for (MultivariatePolynomial<F>& h : H) {
            F leadingCoefficient = h.leadingCoefficient(order);
            h *= F::one / leadingCoefficient;
        }
    }

    Logger::groebnerBasis("🎉 Groebner basis reduction complete");
    Logger::groebnerBasis("📊 Reduced basis size: " + std::to_string(H.size()));
    return H;
}

/**
 * @brief Calculates the reduced Groebner basis of a set of polynomials
 */
template<typename F>
std::vector<MultivariatePolynomial<F>>
    calculateGroebnerBasis(const std::vector<MultivariatePolynomial<F>>& X,
                           const MonomialOrder& order, bool normalizedCoefficients = true) {
    std::vector<MultivariatePolynomial<F>> G = extendToGroebnerBasis(X, order);
    return reduceGroebnerBasis(G, order, normalizedCoefficients);
}

#endif //  GROEBNER_BASIS_HPP
