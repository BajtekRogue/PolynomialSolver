#include "GaloisField.hpp"
#include "Monomial.hpp"
#include "Real.hpp"

Monomial Monomial::null = Monomial(std::map<char, int>{
    {'.', std::numeric_limits<int>::max()}
});

int64_t GaloisField::prime = 2;

double Real::epsilon = 1e-7;
const Real Real::null = Real(std::numeric_limits<double>::infinity());