#ifndef GROEBNER_BASIS_HPP
#define GROEBNER_BASIS_HPP

#include "Monomial.hpp"
#include "MonomialOrders.hpp"
#include "MultivariatePolynomial.hpp"

/**
 * @brief Division algorithm for multivariable polynomials. Size of quotient vector is equal to the
 * size of the divisor vector. Note that the result depends in general on the order of elements in
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
 * Calculates `S(f, g) = lcm(LM(f), LM(g)) * (f / LT(f)  - g / LT(g))`
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

/**
 * Checks if monomials satisfy the lcm criterion in Buchberger's algorithm. That occurs when they
 * are relativly prime and so `lcm(a, b) = a * b`
 */
inline bool lcmCriterion(const Monomial& a, const Monomial& b) {
    return Monomial::lcm(a, b) == a * b;
}

/**
 * Checks if monomials satisfy the chain criterion in Buchberger's algorithm. That occurs when there
 * is a third monomial that divides the lcm of the two monomials
 */
inline bool chainCriterion(const Monomial& a, const Monomial& b,
                           const std::vector<Monomial>& remainingMonomials) {
    for (const Monomial& monomial : remainingMonomials) {
        if (Monomial::divides(Monomial::lcm(a, b), monomial)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Extends set `F` to a Groebner basis using Buchberger's algorithm

 */
template<typename F>
std::vector<MultivariatePolynomial<F>>
    extendToGroebnerBasis(const std::vector<MultivariatePolynomial<F>>& X,
                          const MonomialOrder& order) {

    std::vector<MultivariatePolynomial<F>> G = X;

    while (true) {

        const int n = G.size();
        std::vector<MultivariatePolynomial<F>> H = G;
        bool somethingAdded = false;

        //  Iterate over all pairs (i, j) in G
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {

                const Monomial& i_monomial = G[i].leadingMonomial(order);
                const Monomial& j_monomial = G[j].leadingMonomial(order);

                //  First check lcmCriterion and chainCriterion to avoid doing division
                if (lcmCriterion(i_monomial, j_monomial)) {
                    continue;
                }

                std::vector<Monomial> remainingMonomials(n - (j + 1));
                for (int k = j + 1; k < n; k++) {
                    remainingMonomials[k - (j + 1)] = G[k].leadingMonomial(order);
                }

                if (chainCriterion(i_monomial, j_monomial, remainingMonomials)) {
                    continue;
                }

                //  Need to do division. If r is not 0, add it to H
                MultivariatePolynomial<F> s = syzygy(G[i], G[j], order);
                auto [_, r] = polynomialReduce(s, G, order);

                if (!r.isZeroPolynomial()) {
                    H.push_back(r);
                    somethingAdded = true;
                }
            }
        }

        //  If something was added, update G and continue, otherwise return
        if (!somethingAdded) {
            return H;
        }
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

    std::vector<MultivariatePolynomial<F>> H = G;

    //  First pass: Remove elements that are in the leading terms ideal
    for (const MultivariatePolynomial<F>& g : G) {

        auto it = std::find(H.begin(), H.end(), g);
        if (it != H.end()) {
            H.erase(it);
        }
        else {
            continue;
        }

        bool inLeadingTermsIdeal = false;
        //  If g is in the leading terms ideal generate by other polynomials we don't need it
        for (const MultivariatePolynomial<F>& h : H) {
            if (Monomial::divides(g.leadingMonomial(order), h.leadingMonomial(order))) {
                inLeadingTermsIdeal = true;
                break;
            }
        }

        if (!inLeadingTermsIdeal) {
            H.push_back(g);
        }
    }

    const int currentSize = H.size();
    bool somethingReduced = true;

    //  Second pass: Perform reductions
    while (somethingReduced) {

        somethingReduced = false;
        //  Reduce each polynomial in the basis by the others to simplify it
        for (int i = 0; i < currentSize; i++) {

            std::vector<MultivariatePolynomial<F>> K = H;
            K.erase(K.begin() + i);
            auto [_, r] = polynomialReduce(H[i], K, order);

            if (!r.isZeroPolynomial() && H[i] != r) {
                H[i] = r;
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
    G = reduceGroebnerBasis(G, order, normalizedCoefficients);
    return G;
}

#endif //  GROEBNER_BASIS_HPP
