#include "Logger.hpp"
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
    if (f.degree() == 0) {
        return {};
    }

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

    auto it = std::find_if(coefficients.begin(), coefficients.end(),
                           [](const Rational& r) { return r != Rational::zero; });
    Rational constantTerm = *it;
    Rational leadingTerm = coefficients.back();

    std::vector<int64_t> P = divisors(constantTerm.getNumerator());
    std::vector<int64_t> Q = divisors(leadingTerm.getNumerator());

    std::set<Rational> candidates;
    candidates.insert(Rational::zero);

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
        if (g.evaluate(r) == Rational::zero) {
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
    int degree = f.degree();
    int counter = 0;

    for (int64_t i = 0; i < GaloisField::prime; i++) {
        GaloisField candidate(i);

        if (f.evaluate(candidate) == GaloisField::zero) {
            roots.push_back(candidate);
            counter++;
        }

        if (counter == degree) {
            return roots;
        }
    }
    return roots;
}

std::pair<Real, bool> newton(const UnivariatePolynomial<Real>& f,
                             const UnivariatePolynomial<Real>& df, Real x0) {
    Real x = x0;
    const int maxIt = 1'000'000;

    for (int i = 0; i < maxIt; i++) {
        Real value = f.evaluate(x);
        Real dvalue = df.evaluate(x);

        if (value == Real::zero) {
            return {x, true};
        }

        if (dvalue == Real::zero) {
            //  Try a small perturbation
            x += Real(Real::epsilon * 1'000);
            continue;
        }

        Real xNew = x - value / dvalue;
        if (x == xNew) {
            return {x, true};
        }
        x = xNew;
    }

    return {x, false};
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
        auto [root, succes] = newton(f, df, guess);
        if (succes) {
            potentialRoots.insert(root);
        }
    }

    //  Cluster roots if there are more than they should
    std::vector<Real> rawRoots(potentialRoots.begin(), potentialRoots.end());
    std::sort(rawRoots.begin(), rawRoots.end());

    std::vector<Real> clusteredRoots;
    Real currentCluster = rawRoots.front();

    for (int i = 1; i < rawRoots.size(); i++) {
        if (std::abs(rawRoots[i].getValue() - currentCluster.getValue()) <= Real::epsilon * 100) {
            currentCluster = (currentCluster + rawRoots[i]) / Real(2.0);
        }
        else {
            clusteredRoots.push_back(currentCluster);
            currentCluster = rawRoots[i];
        }
    }
    clusteredRoots.push_back(currentCluster);


    return clusteredRoots;
}

using BigInt = boost::multiprecision::cpp_int;

std::vector<BigInt> divisors(const BigInt& n) {
    BigInt m = boost::multiprecision::abs(n);
    std::vector<BigInt> result;

    if (m == 0 || m == 1) {
        if (m == 1) {
            result.push_back(1);
        }
        return result;
    }

    for (BigInt i = 1; i * i <= m; ++i) {
        if (m % i == 0) {
            result.push_back(i);
            
            BigInt j = m / i;
            if (i != j) {
                result.push_back(j);
            }
        }
    }
    return result;
}


BigInt lcm(const BigInt& a, const BigInt& b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    return boost::multiprecision::abs(a * b) / boost::multiprecision::gcd(a, b);
}

std::vector<BigRational> findBigRationalRoots(const UnivariatePolynomial<BigRational>& f) {
    if (f.degree() == 0) {
        return {};
    }

    std::vector<BigInt> denominators;
    for (const BigRational& coefficient : f.getCoefficients()) {
        denominators.push_back(coefficient.getDenominator());
    }

    BigInt lcm_val = denominators.front();
    for (int i = 1; i < denominators.size(); i++) {
        lcm_val = lcm(lcm_val, denominators[i]);
    }

    UnivariatePolynomial<BigRational> g = f * BigRational(lcm_val);
    std::vector<BigRational> coefficients = g.getCoefficients();
    auto it = std::find_if(coefficients.begin(), coefficients.end(),
                           [](const BigRational& r) { return r != BigRational::zero; });
    BigRational constantTerm = *it;
    BigRational leadingTerm = coefficients.back();
    std::vector<BigInt> P = divisors(constantTerm.getNumerator());
    std::vector<BigInt> Q = divisors(leadingTerm.getNumerator());

    std::set<BigRational> candidates;
    candidates.insert(BigRational::zero);

    for (const BigInt& p : P) {
        for (const BigInt& q : Q) {
            candidates.insert(BigRational(p, q));
            candidates.insert(BigRational(-p, q));
        }
    }

    std::vector<BigRational> roots;
    int degree = g.degree();
    int counter = 0;

    for (const BigRational& r : candidates) {
        if (g.evaluate(r) == BigRational::zero) {
            roots.push_back(r);
            counter++;
        }

        if (counter == degree) {
            return roots;
        }
    }

    return roots;
}
