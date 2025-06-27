#include "GaloisField.hpp"
#include "Monomial.hpp"
#include "Rational.hpp"
#include "Real.hpp"

Monomial Monomial::null = Monomial(std::map<char, int>{
    {'.', std::numeric_limits<int>::max()}
});

const Rational Rational::zero = Rational(0, 1);
const Rational Rational::one = Rational(1, 1);

const GaloisField GaloisField::zero = GaloisField(0);
const GaloisField GaloisField::one = GaloisField(1);

const Real Real::zero = Real(0.0);
const Real Real::one = Real(1.0);

int64_t GaloisField::prime = 2;

double Real::epsilon = 1e-7;