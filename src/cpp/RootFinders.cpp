#include "Solver.hpp"

std::vector<int64_t> divisors(int64_t n) {
    n = std::abs(n);
    std::vector<int64_t> result;

    if (n == 0) {
        return result;
    }
    for (int64_t i = 1; i * i <= n; i++) {
        if (n % i == 0) {
            result.push_back(i);
            if (i != n / i) {
                result.push_back(n / i);
            }
        }
    }
    return result;
};

std::vector<Rational> findRationalRoots(const UnivariatePolynomial<Rational>& f) {

    std::vector<int64_t> denominators;
    for (const Rational& coefficient : f.getCoefficients()) {
        denominators.push_back(coefficient.getDenominator());
    }

    int64_t lcm_val = denominators.front();
    for (int i = 1; i < denominators.size(); i++) {
        lcm_val = std::lcm(lcm_val, denominators[i]);
    }

    UnivariatePolynomial<Rational> g = f * Rational(lcm_val);
    std::vector<Rational> coefficients = g.getCoefficients();
    Rational constantTerm = coefficients.front();
    Rational leadingTerm = coefficients.back();

    std::vector<int64_t> P = divisors(constantTerm.getNumerator());
    std::vector<int64_t> Q = divisors(leadingTerm.getNumerator());

    std::set<Rational> candidates;
    candidates.insert(Rational(0));

    for (int64_t p : P) {
        for (int64_t q : Q) {
            candidates.insert(Rational(p, q));
            candidates.insert(Rational(-p, q));
        }
    }

    std::vector<Rational> roots;
    int degree = g.degree();
    int counter = 0;

    for (const Rational& r : candidates) {
        if (g.evaluate(r) == Rational(0)) {
            roots.push_back(r);
            counter++;
        }

        if (counter == degree) {
            return roots;
        }
    }

    return roots;
}

std::vector<GaloisField> findGaloisFieldRoots(const UnivariatePolynomial<GaloisField>& f) {
    std::vector<GaloisField> roots;
    int64_t prime = GaloisField::prime;
    int degree = f.degree();
    int counter = 0;

    for (int64_t i = 0; i < prime; i++) {
        GaloisField candidate(i);

        if (f.evaluate(candidate) == GaloisField(0)) {
            roots.push_back(candidate);
            counter++;
        }

        if (counter == degree) {
            return roots;
        }
    }
    return roots;
}

Real newton(const UnivariatePolynomial<Real>& f, const UnivariatePolynomial<Real>& df, Real x0) {
    Real x = x0;
    const int maxIt = 1'000'000;

    for (int i = 0; i < maxIt; i++) {
        Real value = f.evaluate(x);
        Real dvalue = df.evaluate(x);

        if (value == Real(0.0)) {
            return x;
        }

        if (dvalue == Real(0.0)) {
            //  Try a small perturbation
            x += Real(Real::epsilon * 1'000);
            continue;
        }

        Real xNew = x - value / dvalue;
        if (x == xNew) {
            return x;
        }
        x = xNew;
    }

    return Real::null;
}

std::vector<Real> findRealRoots(const UnivariatePolynomial<Real>& f) {

    //  Calculate Cauchy Bound
    std::vector<Real> coefficients = f.getCoefficients();
    Real leadingCoeff = coefficients.back();
    double cauchyBound = -1.0;
    for (Real coeff : coefficients) {
        cauchyBound = std::max(cauchyBound, std::abs(coeff.getValue() / leadingCoeff.getValue()));
    }

    //  Find initial guesses in the range
    int cauchyBoundInt = 1 + std::ceil(cauchyBound);
    std::vector<Real> initialGuesses;
    for (int i = -cauchyBoundInt; i <= cauchyBoundInt; i++) {
        initialGuesses.push_back(Real(i));
    }

    //  Apply Newton's method to each initial guess
    std::set<Real> potentialRoots;
    UnivariatePolynomial<Real> df = f.derivative();
    for (const Real& guess : initialGuesses) {
        Real root = newton(f, df, guess);
        if (root != Real::null) {
            potentialRoots.insert(root);
        }
    }

    std::vector<Real> roots(potentialRoots.begin(), potentialRoots.end());
    return roots;
}