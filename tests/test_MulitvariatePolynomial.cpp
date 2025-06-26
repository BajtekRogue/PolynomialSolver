#include "GroebnerBasis.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"

#include <gtest/gtest.h>

class MultivariatePolynomialTest : public ::testing::Test {
protected:
    void SetUp() override {
        //  Define common variables
        x = defineVariable<Rational>('x');
        y = defineVariable<Rational>('y');
        z = defineVariable<Rational>('z');

        zeroMultivariatePolynomial = MultivariatePolynomial<Rational>();

        //  Simple MultivariatePolynomials for reuse
        p1 = x * 2 + y * 3;             //  2x + 3y
        p2 = (x ^ 2) + (y ^ 2) + 5;     //  x² + y² + 5
        p3 = (x ^ 3) + x * y * y + 5;   //  x³ + xy² + 5
        p4 = x + y;                     //  x + y
        p5 = (x ^ 2) + 3 * (y ^ 2) + 5; //  x² + 3y² + 5

        //  Complex MultivariatePolynomial for evaluation/substitution tests
        complexPoly = -4 * (x ^ 2) + 30 * x * y * z + (z ^ 111) + 5 * x * y;

        //  MultivariatePolynomial for degree testing
        degreePoly = (x ^ 2) + x * y * z + (z ^ 111) + 5 * x * y;

        //  MultivariatePolynomial for substitution
        substitutePoly = (x ^ 2) + x * y + 5;

        //  Expected coefficient map
        expectedCoefficients = {
            {Monomial(), Rational(1)},
            {Monomial(std::map<char, int>{{'x', 1}}), Rational(2)},
            {Monomial(std::map<char, int>{{'y', 2}}), Rational(3)}
        };

        //  Evaluation values
        evalValues = {
            {'x', Rational(1)},
            {'y', Rational(2)},
            {'z', Rational(0)}
        };
        incompleteValues = {
            {'x', Rational(1)}
        };
    }

    void TearDown() override {
    }

    //  Test fixture members - reusable variables and MultivariatePolynomials
    MultivariatePolynomial<Rational> x, y, z;
    MultivariatePolynomial<Rational> zeroMultivariatePolynomial;
    MultivariatePolynomial<Rational> p1, p2, p3, p4, p5;
    MultivariatePolynomial<Rational> complexPoly;
    MultivariatePolynomial<Rational> degreePoly;
    MultivariatePolynomial<Rational> substitutePoly;
    std::map<Monomial, Rational> expectedCoefficients;
    std::map<char, Rational> evalValues;
    std::map<char, Rational> incompleteValues;
};

TEST_F(MultivariatePolynomialTest, DefaultConstructor) {
    EXPECT_TRUE(zeroMultivariatePolynomial.isZeroPolynomial());
}

TEST_F(MultivariatePolynomialTest, Constructor) {
    MultivariatePolynomial<Rational> p = MultivariatePolynomial<Rational>(expectedCoefficients);
    EXPECT_EQ(p.getCoefficients(), expectedCoefficients);
}

TEST_F(MultivariatePolynomialTest, Add1) {
    std::map<Monomial, Rational> coefficients = {
        {Monomial(std::map<char, int>{{'x', 1}}), Rational(2)},
        {Monomial(std::map<char, int>{{'y', 1}}), Rational(3)}
    };
    MultivariatePolynomial<Rational> expected = MultivariatePolynomial<Rational>(coefficients);
    EXPECT_EQ(p1, expected);
}

TEST_F(MultivariatePolynomialTest, Add2) {
    MultivariatePolynomial<Rational> q = -(x * x) + 3 * y * y;
    EXPECT_EQ(p2 + q, 4 * y * y + 5);
}

TEST_F(MultivariatePolynomialTest, AddInPlace) {
    MultivariatePolynomial<Rational> temp = p3; //  x³ + xy² + 5
    temp += x * y * y - 5;
    EXPECT_EQ(temp, (x ^ 3) + 2 * x * y * y);
}

TEST_F(MultivariatePolynomialTest, Sub1) {
    MultivariatePolynomial<Rational> p = x * 2 - y * 3;
    std::map<Monomial, Rational> coefficients = {
        {Monomial(std::map<char, int>{{'x', 1}}), Rational(2) },
        {Monomial(std::map<char, int>{{'y', 1}}), Rational(-3)}
    };
    MultivariatePolynomial<Rational> expected = MultivariatePolynomial<Rational>(coefficients);
    EXPECT_EQ(p, expected);
}

TEST_F(MultivariatePolynomialTest, Sub2) {
    MultivariatePolynomial<Rational> q = (x * x) + 3 * y * y;
    EXPECT_EQ(p2 - q, -2 * y * y + 5);
}

TEST_F(MultivariatePolynomialTest, SubInPlace) {
    MultivariatePolynomial<Rational> temp = p3; //  x³ + xy² + 5
    temp -= x * y * y - 5;
    EXPECT_EQ(temp, (x ^ 3) + 10);
}

TEST_F(MultivariatePolynomialTest, Multiplication1) {
    auto result = p5 * p4; //  (x² + 3y² + 5) * (x + y)
    auto expected = (x ^ 3) + (x ^ 2) * y + 3 * x * (y ^ 2) + 5 * x + 5 * y + 3 * (y ^ 3);
    EXPECT_EQ(result, expected);
}

TEST_F(MultivariatePolynomialTest, Multiplication2) {
    auto result = p5 * 6; //  (x² + 3y² + 5) * 6
    auto expected = 6 * (x ^ 2) + 18 * (y ^ 2) + 30;
    EXPECT_EQ(result, expected);
}

TEST_F(MultivariatePolynomialTest, Multiplication3) {
    auto result = p5 * 0; //  (x² + 3y² + 5) * 0
    EXPECT_EQ(result, MultivariatePolynomial<Rational>(Rational(0)));
}

TEST_F(MultivariatePolynomialTest, MultiplicationInPlace) {
    MultivariatePolynomial<Rational> temp = p3; //  x³ + xy² + 5
    temp *= x * y * y - 5;
    EXPECT_EQ(temp, (x ^ 4) * (y ^ 2) - 5 * (x ^ 3) + (x ^ 2) * (y ^ 4) - 25);
}

TEST_F(MultivariatePolynomialTest, EvaluateError) {
    EXPECT_THROW(y.evaluate(incompleteValues), std::invalid_argument);
}

TEST_F(MultivariatePolynomialTest, Evaluate) {
    EXPECT_EQ(complexPoly.evaluate(evalValues), Rational(6));
}

TEST_F(MultivariatePolynomialTest, TotalDegree) {
    EXPECT_EQ(degreePoly.totalDegree(), 111);
}

TEST_F(MultivariatePolynomialTest, Substitute) {
    MultivariatePolynomial<Rational> expected = 3 * y + 14;
    EXPECT_EQ(substitutePoly.substitute('x', Rational(3)), expected);
}

