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

/* ===================CLAUDE VERSION ====================*/
// using BigInt = boost::multiprecision::cpp_int;

// //  Optimized divisors function
// std::vector<BigInt> divisors(const BigInt& n) {
//     BigInt m = boost::multiprecision::abs(n);
//     std::vector<BigInt> result;

//     if (m == 0 || m == 1) {
//         if (m == 1) {
//             result.push_back(1);
//         }    
//         return result;
//     }

//     //  Reserve space to avoid reallocations
//     result.reserve(20); //  Reasonable initial size

//     //  Special case for 2 to avoid even number iterations
//     if (m % 2 == 0) {
//         result.push_back(2);
//         BigInt half = m / 2;
//         if (half != 2) {
//             result.push_back(half);
//         }
//     }

//     //  Only check odd numbers starting from 3
//     BigInt start = (m % 2 == 0) ? 3 : 1;
//     for (BigInt i = start; i * i <= m; i += 2) {
//         if (m % i == 0) {
//             result.push_back(i);

//             BigInt j = m / i;
//             if (i != j) {
//                 result.push_back(j);
//             }
//         }
//     }

//     //  Always include 1 and m itself
//     result.push_back(1);
//     if (m != 1) {
//         result.push_back(m);
//     }

//     return result;
// }

// //  Inline wrapper for GCD
// inline BigInt gcd(const BigInt& a, const BigInt& b) {
//     return boost::multiprecision::gcd(a, b);
// }

// //  Optimized LCM
// BigInt lcm(const BigInt& a, const BigInt& b) {
//     if (a == 0 || b == 0) {
//         return 0;
//     }

//     //  Avoid computing abs twice
//     BigInt abs_a = boost::multiprecision::abs(a);
//     BigInt abs_b = boost::multiprecision::abs(b);
//     BigInt gcd_val = boost::multiprecision::gcd(abs_a, abs_b);

//     //  Divide first to avoid overflow in intermediate multiplication
//     return (abs_a / gcd_val) * abs_b;
// }

// //  Optimized root finding function
// std::vector<BigRational> findBigRationalRoots(const UnivariatePolynomial<BigRational>& f) {
//     if (f.degree() == 0) {
//         return {};
//     }

//     const auto& coefficients = f.getCoefficients();

//     //  Compute LCM of denominators more efficiently
//     BigInt lcm_val = 1;
//     for (const BigRational& coefficient : coefficients) {
//         const BigInt& denom = coefficient.getDenominator();
//         if (denom != 1) { //  Skip if denominator is 1
//             lcm_val = lcm(lcm_val, denom);
//         }
//     }

//     //  Early exit if already integer polynomial
//     if (lcm_val == 1) {
//         //  Use original polynomial if already integer
//         return findBigRationalRootsIntegerPoly(f);
//     }

//     //  Create integer polynomial
//     UnivariatePolynomial<BigRational> g = f * BigRational(lcm_val);
//     return findBigRationalRootsIntegerPoly(g);
// }

// //  Separate function for integer polynomial root finding
// std::vector<BigRational>
//     findBigRationalRootsIntegerPoly(const UnivariatePolynomial<BigRational>& g) {
//     const auto& coefficients = g.getCoefficients();

//     //  Find first non-zero coefficient (constant term)
//     auto it = std::find_if(coefficients.begin(), coefficients.end(),
//                            [](const BigRational& r) { return r != BigRational::zero; });

//     if (it == coefficients.end()) {
//         return {}; //  All coefficients are zero
//     }

//     const BigRational& constantTerm = *it;
//     const BigRational& leadingTerm = coefficients.back();

//     //  Get numerators directly to avoid extra copies
//     const BigInt& constant_num = constantTerm.getNumerator();
//     const BigInt& leading_num = leadingTerm.getNumerator();

//     //  Early exit for simple cases
//     if (constant_num == 0) {
//         std::vector<BigRational> roots;
//         roots.push_back(BigRational::zero);
//         return roots;
//     }

//     std::vector<BigInt> P = divisors(constant_num);
//     std::vector<BigInt> Q = divisors(leading_num);

//     //  Use unordered_set for better performance
//     std::unordered_set<BigRational> candidate_set;
//     candidate_set.reserve(P.size() * Q.size() * 2 + 1);

//     //  Always test zero
//     candidate_set.insert(BigRational::zero);

//     //  Generate candidates more efficiently
//     for (const BigInt& p : P) {
//         for (const BigInt& q : Q) {
//             if (p != 0 && q != 0) { //  Safety check
//                 candidate_set.insert(BigRational(p, q));
//                 if (p != 0) {       //  Avoid -0
//                     candidate_set.insert(BigRational(-p, q));
//                 }
//             }
//         }
//     }

//     std::vector<BigRational> roots;
//     roots.reserve(g.degree()); //  Reserve space for maximum possible roots

//     int degree = g.degree();

//     //  Test candidates
//     for (const BigRational& candidate : candidate_set) {
//         if (g.evaluate(candidate) == BigRational::zero) {
//             roots.push_back(candidate);

//             //  Early termination when we find all roots
//             if (roots.size() == degree) {
//                 break;
//             }
//         }
//     }

//     return roots;
// }

// //  Alternative approach: use integer arithmetic when possible
// std::vector<BigRational>
//     findBigRationalRootsIntegerOpt(const UnivariatePolynomial<BigRational>& f) {
//     //  Check if all coefficients are integers
//     bool all_integers = true;
//     for (const BigRational& coeff : f.getCoefficients()) {
//         if (!coeff.isInteger()) {
//             all_integers = false;
//             break;
//         }
//     }

//     if (all_integers) {
//         //  Use integer polynomial evaluation for better performance
//         return findRootsIntegerPolynomial(f);
//     }
//     else {
//         return findBigRationalRoots(f);
//     }
// }

// //  Specialized version for integer polynomials
// std::vector<BigRational> findRootsIntegerPolynomial(const UnivariatePolynomial<BigRational>& f) {
//     const auto& coefficients = f.getCoefficients();

//     //  Convert to integer coefficients for faster evaluation
//     std::vector<BigInt> int_coeffs;
//     int_coeffs.reserve(coefficients.size());

//     for (const BigRational& coeff : coefficients) {
//         int_coeffs.push_back(coeff.getNumerator());
//     }

//     //  Find constant and leading terms
//     auto it =
//         std::find_if(int_coeffs.begin(), int_coeffs.end(), [](const BigInt& x) { return x != 0; });

//     if (it == int_coeffs.end()) {
//         return {};
//     }

//     const BigInt& constant_term = *it;
//     const BigInt& leading_term = int_coeffs.back();

//     std::vector<BigInt> P = divisors(boost::multiprecision::abs(constant_term));
//     std::vector<BigInt> Q = divisors(boost::multiprecision::abs(leading_term));

//     std::vector<BigRational> roots;
//     roots.reserve(f.degree());

//     //  Test candidates using integer arithmetic
//     for (const BigInt& p : P) {
//         for (const BigInt& q : Q) {
//             //  Test p/q
//             if (evaluateIntegerPoly(int_coeffs, p, q) == 0) {
//                 roots.emplace_back(p, q);
//                 if (roots.size() == f.degree()) {
//                     return roots;
//                 }
//             }

//             //  Test -p/q (avoid -0)
//             if (p != 0 && evaluateIntegerPoly(int_coeffs, -p, q) == 0) {
//                 roots.emplace_back(-p, q);
//                 if (roots.size() == f.degree()) {
//                     return roots;
//                 }
//             }
//         }
//     }

//     return roots;
// }

// //  Fast integer polynomial evaluation
// BigInt evaluateIntegerPoly(const std::vector<BigInt>& coeffs, const BigInt& num,
//                            const BigInt& den) {
//     if (coeffs.empty()) {
//         return 0;
//     }

//     //  Use Horner's method with rational arithmetic
//     //  Evaluate p(num/den) * den^degree to avoid fractions
//     BigInt result = coeffs.back();
//     BigInt den_power = 1;

//     for (int i = coeffs.size() - 2; i >= 0; --i) {
//         den_power *= den;
//         result = result * num + coeffs[i] * den_power;
//     }

//     return result;
// }