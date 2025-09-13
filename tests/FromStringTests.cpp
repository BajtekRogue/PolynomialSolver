#include "GaloisField.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"
#include "Real.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

class FromStringTests : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        GaloisField::setPrime(11);
    }

    void SetUp() override {
        a = defineVariable<GaloisField>('a');
        b = defineVariable<GaloisField>('b');
        c = defineVariable<GaloisField>('c');

        x = defineVariable<Rational>('x');
        y = defineVariable<Rational>('y');
        z = defineVariable<Rational>('z');
    }

    MultivariatePolynomial<Rational> x;
    MultivariatePolynomial<Rational> y;
    MultivariatePolynomial<Rational> z;
    MultivariatePolynomial<GaloisField> a;
    MultivariatePolynomial<GaloisField> b;
    MultivariatePolynomial<GaloisField> c;
};

TEST_F(FromStringTests, StringConstructorSimpleFraction) {
    Rational r("3/4");
    EXPECT_EQ(r.getNumerator(), 3);
    EXPECT_EQ(r.getDenominator(), 4);
}

TEST_F(FromStringTests, StringConstructorNegativeNumerator) {
    Rational r("-3/4");
    EXPECT_EQ(r.getNumerator(), -3);
    EXPECT_EQ(r.getDenominator(), 4);
}

TEST_F(FromStringTests, StringConstructorNegativeDenominatorThrows) {
    EXPECT_THROW(Rational("3/-4"), std::invalid_argument);
}

TEST_F(FromStringTests, StringConstructorPositiveDenominatorWithPlusThrows) {
    EXPECT_THROW(Rational("3/+4"), std::invalid_argument);
}

TEST_F(FromStringTests, StringConstructorNegativeBothThrows) {
    EXPECT_THROW(Rational("-3/-5"), std::invalid_argument);
}

TEST_F(FromStringTests, StringConstructorNoDenominator) {
    Rational r("5");
    EXPECT_EQ(r.getNumerator(), 5);
    EXPECT_EQ(r.getDenominator(), 1);
}

TEST_F(FromStringTests, StringConstructorZeroNumerator) {
    Rational r("0/15");
    EXPECT_EQ(r.getNumerator(), 0);
    EXPECT_EQ(r.getDenominator(), 1);
}

TEST_F(FromStringTests, StringConstructorZeroDenominatorThrows) {
    EXPECT_THROW(Rational("3/0"), std::invalid_argument);
}

TEST_F(FromStringTests, StringConstructorInvalidFormatThrows) {
    EXPECT_THROW(Rational("abc/def"), std::invalid_argument);
    EXPECT_THROW(Rational("3//4"), std::invalid_argument);
    EXPECT_THROW(Rational("3/"), std::invalid_argument);
    EXPECT_THROW(Rational("/4"), std::invalid_argument);
    EXPECT_THROW(Rational("12/3b"), std::invalid_argument);
}

TEST_F(FromStringTests, StringConstructorExtraSignsThrows) {
    EXPECT_THROW(Rational("--3/4"), std::invalid_argument);
    EXPECT_THROW(Rational("++3/4"), std::invalid_argument);
    EXPECT_THROW(Rational("3/--4"), std::invalid_argument);
    EXPECT_THROW(Rational("3/++4"), std::invalid_argument);
}

TEST_F(FromStringTests, ConstructorWithValidString) {
    GaloisField a("5");
    GaloisField b("-10");
    GaloisField c("50");
    EXPECT_EQ(a, GaloisField(5));
    EXPECT_EQ(b, GaloisField(1));
    EXPECT_EQ(c, GaloisField(6));
}

TEST_F(FromStringTests, ConstructorWithInvalidStringThrows) {
    EXPECT_THROW(GaloisField("abc"), std::invalid_argument);
    EXPECT_THROW(GaloisField("123abc"), std::invalid_argument);
    EXPECT_THROW(GaloisField("--a"), std::invalid_argument);
    EXPECT_THROW(GaloisField(""), std::invalid_argument);
}

TEST_F(FromStringTests, RealConstructorValidPositive) {
    Real r("3.14");
    EXPECT_DOUBLE_EQ(r.getValue(), 3.14);
}

TEST_F(FromStringTests, RealConstructorValidNegative) {
    Real r("-2.718");
    EXPECT_DOUBLE_EQ(r.getValue(), -2.718);
}

TEST_F(FromStringTests, RealConstructorZero) {
    Real r("0");
    EXPECT_DOUBLE_EQ(r.getValue(), 0.0);
}

TEST_F(FromStringTests, RealConstructorScientificNotation) {
    Real r1("1e3");
    Real r2("-2.5e-2");
    EXPECT_DOUBLE_EQ(r1.getValue(), 1000.0);
    EXPECT_DOUBLE_EQ(r2.getValue(), -0.025);
}

TEST_F(FromStringTests, RealConstructorLeadingPlus) {
    Real r("+42.0");
    EXPECT_DOUBLE_EQ(r.getValue(), 42.0);
}

TEST_F(FromStringTests, RealConstructorInvalidFormatThrows) {
    EXPECT_THROW(Real("abc"), std::invalid_argument);
    EXPECT_THROW(Real("3.1.4"), std::invalid_argument);
    EXPECT_THROW(Real("2e"), std::invalid_argument);
    EXPECT_THROW(Real("--3.14"), std::invalid_argument);
    EXPECT_THROW(Real(""), std::invalid_argument);
}

TEST_F(FromStringTests, RealConstructorVeryLargeValue) {
    Real r("1e308");
    EXPECT_DOUBLE_EQ(r.getValue(), 1e308);
}

TEST_F(FromStringTests, RealConstructorVerySmallValue) {
    Real r("1e-30");
    EXPECT_DOUBLE_EQ(r.getValue(), 1e-30);
}

TEST_F(FromStringTests, MonomialConstructorSingleVariables) {
    Monomial m("x");
    auto expected = std::map<char, int>{
        {'x', 1}
    };
    EXPECT_EQ(m.getMonomial(), expected);
    EXPECT_EQ(m.getDegree(), 1);
    EXPECT_EQ(m.getNumVariables(), 1);
}

TEST_F(FromStringTests, MonomialConstructorMultipleVariablesNoExponents) {
    Monomial m("xyz");
    auto expected = std::map<char, int>{
        {'x', 1},
        {'y', 1},
        {'z', 1}
    };
    EXPECT_EQ(m.getMonomial(), expected);
    EXPECT_EQ(m.getDegree(), 3);
    EXPECT_EQ(m.getNumVariables(), 3);
}

TEST_F(FromStringTests, MonomialConstructorWithExponents) {
    Monomial m("x^2y^3z^1");
    auto expected = std::map<char, int>{
        {'x', 2},
        {'y', 3},
        {'z', 1}
    };
    EXPECT_EQ(m.getMonomial(), expected);
    EXPECT_EQ(m.getDegree(), 6);
    EXPECT_EQ(m.getNumVariables(), 3);
}

TEST_F(FromStringTests, MonomialConstructorMixedWithAndWithoutExponents) {
    Monomial m("x^2y^12z^553");
    auto expected = std::map<char, int>{
        {'x', 2  },
        {'y', 12  },
        {'z', 553}
    };
    EXPECT_EQ(m.getMonomial(), expected);
    EXPECT_EQ(m.getDegree(), 2 + 12 + 553);
    EXPECT_EQ(m.getNumVariables(), 3);
}

TEST_F(FromStringTests, MonomialConstructorEmptyString) {
    Monomial m("");
    auto expected = std::map<char, int>{};
    EXPECT_EQ(m.getMonomial(), expected);
    EXPECT_EQ(m.getDegree(), 0);
    EXPECT_EQ(m.getNumVariables(), 0);
}

TEST_F(FromStringTests, MonomialConstructorInvalidCharacterThrows) {
    EXPECT_THROW(Monomial("x^2y#z^3"), std::invalid_argument);
    EXPECT_THROW(Monomial("3x^2"), std::invalid_argument);
    EXPECT_THROW(Monomial("x^2y^"), std::invalid_argument);
    EXPECT_THROW(Monomial("x^^2"), std::invalid_argument);
    EXPECT_THROW(Monomial("x^2y^3x^1"), std::invalid_argument);
}

TEST_F(FromStringTests, MonomialConstructorInvalidExponentThrows) {
    EXPECT_THROW(Monomial("x^-2"), std::invalid_argument);
    EXPECT_THROW(Monomial("y^0"), std::invalid_argument);
}

TEST_F(FromStringTests, RationalPolynomials) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("3x^2+ 5xy - 7z^3y"),
              3 * (x ^ 2) + 5 * x * y - 7 * y * (z ^ 3));
}


TEST_F(FromStringTests, PolynomialSimpleRational) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("3/2x^20 + 5y - 7/3"),
              Rational(3, 2) * (x ^ 20) + 5 * y - Rational(7, 3));
}

TEST_F(FromStringTests, PolynomialImplicitCoefficients) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("x + y - z"), x + y - z);
}

TEST_F(FromStringTests, PolynomialImplicitOneCoefficient) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("+x - y"), x - y);
}

TEST_F(FromStringTests, PolynomialNegativeCoefficients) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("-2x + 3y - 1/3z^2"),
              -2 * x + 3 * y - Rational(1, 3) * (z ^ 2));
}

TEST_F(FromStringTests, PolynomialConstantTerm) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("5"), MultivariatePolynomial<Rational>(Rational(5)));
}

TEST_F(FromStringTests, PolynomialConstantTermWithVariables) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("3x^2 + 5"), 3 * (x ^ 2) + 5);
}

TEST_F(FromStringTests, PolynomialFractionalCoefficients) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("1/2x + 3/4y^2 - 5/6z"),
              Rational(1, 2) * x + Rational(3, 4) * (y ^ 2) - Rational(5, 6) * z);
}

TEST_F(FromStringTests, PolynomialZeroCoefficient) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("0x + 3y"), 3 * y);
}

TEST_F(FromStringTests, PolynomialSingleVariable) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("x"), x);
}

TEST_F(FromStringTests, PolynomialComplexExpression) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("x^3y^2 + 2xy - 3x + 7"),
              (x ^ 3) * (y ^ 2) + 2 * x * y - 3 * x + 7);
}

TEST_F(FromStringTests, PolynomialWithSpaces) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("  3 x^2  +   5 y  -  7  "),
              3 * (x ^ 2) + 5 * y - 7);
}

TEST_F(FromStringTests, GaloisFieldPolynomialBasic) {
    EXPECT_EQ(MultivariatePolynomial<GaloisField>("3a^2 + 5b - 7c"), 3 * (a ^ 2) + 5 * b - 7 * c);
}

TEST_F(FromStringTests, GaloisFieldPolynomialWithModularArithmetic) {
    //  With prime = 11: 15 ≡ 4 (mod 11), 22 ≡ 0 (mod 11)
    EXPECT_EQ(MultivariatePolynomial<GaloisField>("15a + 22b + 33c"), 4 * a + 0 * b + 0 * c);
}

TEST_F(FromStringTests, GaloisFieldPolynomialNegativeCoefficients) {
    //  With prime = 11: -5 ≡ 6 (mod 11), -12 ≡ 10 (mod 11)
    EXPECT_EQ(MultivariatePolynomial<GaloisField>("-5a + 3b - 12c"), 6 * a + 3 * b + 10 * c);
}

TEST_F(FromStringTests, GaloisFieldPolynomialImplicitCoefficients) {
    EXPECT_EQ(MultivariatePolynomial<GaloisField>("a - b + c"), a - b + c);
}

TEST_F(FromStringTests, PolynomialEmptyString) {
    EXPECT_THROW(MultivariatePolynomial<Rational>(""), std::invalid_argument);
}

TEST_F(FromStringTests, PolynomialOnlySpaces) {
    EXPECT_THROW(MultivariatePolynomial<Rational>("   "), std::invalid_argument);
}

TEST_F(FromStringTests, PolynomialLikeTerms) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("3x + 2x"), 5 * x);
}

TEST_F(FromStringTests, PolynomialComplexLikeTerms) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("2x^2y + 3x^2y"), 5 * (x ^ 2) * y);
}

TEST_F(FromStringTests, PolynomialLikeTermsWithDifferentSigns) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("5x - 2x"), 3 * x);
    EXPECT_EQ(MultivariatePolynomial<GaloisField>("5a - 2a"), 3 * a);
}

TEST_F(FromStringTests, PolynomialLikeTermsResultingInZero) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("3x - 3x"), MultivariatePolynomial<Rational>());
}

TEST_F(FromStringTests, PolynomialStartingWithPlus) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("+3x - 2y"), 3 * x - 2 * y);
}

TEST_F(FromStringTests, PolynomialStartingWithMinus) {
    EXPECT_EQ(MultivariatePolynomial<Rational>("-3x + 2y"), -3 * x + 2 * y);
}

TEST_F(FromStringTests, PolynomialInvalidFormat) {
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x^"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x + + 2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x -- 2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x +- 2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x -+ 2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x+++++2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x-----2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x+-+-+2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3x2y"), std::invalid_argument);
}

TEST_F(FromStringTests, PolynomialInvalidRationalCoefficient) {
    EXPECT_THROW(MultivariatePolynomial<Rational>("3/0x + 2y"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<Rational>("3//2x + y"), std::invalid_argument);
}

TEST_F(FromStringTests, PolynomialInvalidGaloisFieldCoefficient) {
    EXPECT_THROW(MultivariatePolynomial<GaloisField>("abca + 2b"), std::invalid_argument);
    EXPECT_THROW(MultivariatePolynomial<GaloisField>("3.5a + b"), std::invalid_argument);
}

TEST_F(FromStringTests, ReductionToGalois) {
    auto f = MultivariatePolynomial<GaloisField>("a - 121b^7 + 22");
    auto g = a;
    EXPECT_EQ(f, g);
}