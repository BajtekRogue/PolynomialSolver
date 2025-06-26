#include "Rational.hpp"
#include "UnivariatePolynomial.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

class UnivariatePolynomialTest : public ::testing::Test {
protected:
    void SetUp() override {
        zeroPoly = UnivariatePolynomial<Rational>();
        onePoly = UnivariatePolynomial<Rational>(Rational(1));

        std::vector<Rational> coeffs1 = {Rational(1), Rational(2), Rational(3)};
        p1 = UnivariatePolynomial<Rational>(coeffs1);

        std::vector<Rational> coeffs2 = {Rational(2), Rational(1)};
        p2 = UnivariatePolynomial<Rational>(coeffs2);

        std::vector<Rational> coeffs3 = {Rational(0), Rational(0), Rational(1)};
        p3 = UnivariatePolynomial<Rational>(coeffs3);

        std::vector<Rational> coeffs4 = {Rational(1, 2), Rational(1, 3)};
        p4 = UnivariatePolynomial<Rational>(coeffs4);

        std::vector<Rational> coeffs5 = {Rational(5), Rational(7), Rational(0) , Rational(-1), Rational(2)};
        p5 = UnivariatePolynomial<Rational>(coeffs5);
    }

    UnivariatePolynomial<Rational> zeroPoly;
    UnivariatePolynomial<Rational> onePoly;
    UnivariatePolynomial<Rational> p1;
    UnivariatePolynomial<Rational> p2;
    UnivariatePolynomial<Rational> p3;
    UnivariatePolynomial<Rational> p4;
    UnivariatePolynomial<Rational> p5;
};

TEST_F(UnivariatePolynomialTest, DefaultConstructor) {
    UnivariatePolynomial<Rational> p;
    EXPECT_TRUE(p.isZero());
    EXPECT_EQ(p.degree(), 0);
}

TEST_F(UnivariatePolynomialTest, ConstantConstructor) {
    UnivariatePolynomial<Rational> p(Rational(5));
    EXPECT_FALSE(p.isZero());
    EXPECT_TRUE(p.isConstant());
    EXPECT_EQ(p.degree(), 0);
    EXPECT_EQ(p[0], Rational(5));
}

TEST_F(UnivariatePolynomialTest, VectorConstructor) {
    std::vector<Rational> coeffs = {Rational(1), Rational(2), Rational(3)};
    UnivariatePolynomial<Rational> p(coeffs);
    EXPECT_EQ(p.degree(), 2);
    EXPECT_EQ(p[0], Rational(1));
    EXPECT_EQ(p[1], Rational(2));
    EXPECT_EQ(p[2], Rational(3));
}

TEST_F(UnivariatePolynomialTest, EmptyVectorConstructor) {
    std::vector<Rational> empty_coeffs;
    UnivariatePolynomial<Rational> p(empty_coeffs);
    EXPECT_TRUE(p.isZero());
    EXPECT_EQ(p.degree(), 0);
}

TEST_F(UnivariatePolynomialTest, LeadingZeroRemoval) {
    std::vector<Rational> coeffs = {Rational(1), Rational(2), Rational(0), Rational(0)};
    UnivariatePolynomial<Rational> p(coeffs);
    EXPECT_EQ(p.degree(), 1);
    EXPECT_EQ(p[0], Rational(1));
    EXPECT_EQ(p[1], Rational(2));
}

TEST_F(UnivariatePolynomialTest, IndexOperatorRead) {
    EXPECT_EQ(p1[0], Rational(1));
    EXPECT_EQ(p1[1], Rational(2));
    EXPECT_EQ(p1[2], Rational(3));
    EXPECT_EQ(p1[10], Rational(0));
}

TEST_F(UnivariatePolynomialTest, IndexOperatorWrite) {
    UnivariatePolynomial<Rational> p = p1;
    p[1] = Rational(5);
    EXPECT_EQ(p[1], Rational(5));

    p[5] = Rational(7);
    EXPECT_EQ(p[5], Rational(7));
    EXPECT_EQ(p.degree(), 5);
}

TEST_F(UnivariatePolynomialTest, LeadingCoefficient) {
    EXPECT_EQ(p1.leadingCoefficient(), Rational(3));
    EXPECT_EQ(p2.leadingCoefficient(), Rational(1));
    EXPECT_EQ(zeroPoly.leadingCoefficient(), Rational(0));
}

TEST_F(UnivariatePolynomialTest, AdditionBasic) {
    UnivariatePolynomial<Rational> result = p1 + p2;
    EXPECT_EQ(result[0], Rational(3));
    EXPECT_EQ(result[1], Rational(3));
    EXPECT_EQ(result[2], Rational(3));
    EXPECT_EQ(result.degree(), 2);
}

TEST_F(UnivariatePolynomialTest, AdditionWithZero) {
    UnivariatePolynomial<Rational> result = p1 + zeroPoly;
    EXPECT_EQ(result, p1);
}

TEST_F(UnivariatePolynomialTest, AdditionCommutative) {
    EXPECT_EQ(p1 + p2, p2 + p1);
}

TEST_F(UnivariatePolynomialTest, AdditionWithScalar) {
    UnivariatePolynomial<Rational> result = p1 + Rational(5);
    EXPECT_EQ(result[0], Rational(6));
    EXPECT_EQ(result[1], Rational(2));
    EXPECT_EQ(result[2], Rational(3));
}

TEST_F(UnivariatePolynomialTest, AdditionAssignment) {
    UnivariatePolynomial<Rational> p = p1;
    p += p2;
    EXPECT_EQ(p[0], Rational(3));
    EXPECT_EQ(p[1], Rational(3));
    EXPECT_EQ(p[2], Rational(3));
}

TEST_F(UnivariatePolynomialTest, SubtractionBasic) {
    UnivariatePolynomial<Rational> result = p1 - p2;
    EXPECT_EQ(result[0], Rational(-1));
    EXPECT_EQ(result[1], Rational(1));
    EXPECT_EQ(result[2], Rational(3));
}

TEST_F(UnivariatePolynomialTest, SubtractionWithZero) {
    UnivariatePolynomial<Rational> result = p1 - zeroPoly;
    EXPECT_EQ(result, p1);
}

TEST_F(UnivariatePolynomialTest, SubtractionWithSelf) {
    UnivariatePolynomial<Rational> result = p1 - p1;
    EXPECT_TRUE(result.isZero());
}

TEST_F(UnivariatePolynomialTest, SubtractionWithScalar) {
    UnivariatePolynomial<Rational> result = p1 - Rational(1);
    EXPECT_EQ(result[0], Rational(0));
    EXPECT_EQ(result[1], Rational(2));
    EXPECT_EQ(result[2], Rational(3));
}

TEST_F(UnivariatePolynomialTest, MultiplicationBasic) {
    UnivariatePolynomial<Rational> result = p2 * p3;
    EXPECT_EQ(result[0], Rational(0));
    EXPECT_EQ(result[1], Rational(0));
    EXPECT_EQ(result[2], Rational(2));
    EXPECT_EQ(result[3], Rational(1));
    EXPECT_EQ(result.degree(), 3);
}

TEST_F(UnivariatePolynomialTest, MultiplicationWithZero) {
    UnivariatePolynomial<Rational> result = p1 * zeroPoly;
    EXPECT_TRUE(result.isZero());
}

TEST_F(UnivariatePolynomialTest, MultiplicationWithOne) {
    UnivariatePolynomial<Rational> result = p1 * onePoly;
    EXPECT_EQ(result, p1);
}

TEST_F(UnivariatePolynomialTest, MultiplicationCommutative) {
    EXPECT_EQ(p1 * p2, p2 * p1);
}

TEST_F(UnivariatePolynomialTest, MultiplicationWithScalar) {
    UnivariatePolynomial<Rational> result = p1 * Rational(2);
    EXPECT_EQ(result[0], Rational(2));
    EXPECT_EQ(result[1], Rational(4));
    EXPECT_EQ(result[2], Rational(6));
}

TEST_F(UnivariatePolynomialTest, MultiplicationWithFractionalScalar) {
    UnivariatePolynomial<Rational> result = p1 * Rational(1, 2);
    EXPECT_EQ(result[0], Rational(1, 2));
    EXPECT_EQ(result[1], Rational(1));
    EXPECT_EQ(result[2], Rational(3, 2));
}

TEST_F(UnivariatePolynomialTest, DivisionByScalar) {
    UnivariatePolynomial<Rational> result = p1 / Rational(2);
    EXPECT_EQ(result[0], Rational(1, 2));
    EXPECT_EQ(result[1], Rational(1));
    EXPECT_EQ(result[2], Rational(3, 2));
}

TEST_F(UnivariatePolynomialTest, DivisionByZeroScalar) {
    EXPECT_THROW(p1 / Rational(0), std::invalid_argument);
}

TEST_F(UnivariatePolynomialTest, PolynomialDivision) {
    std::vector<Rational> coeffsDividend = {Rational(-1), Rational(0), Rational(1)};
    UnivariatePolynomial<Rational> dividend(coeffsDividend);

    std::vector<Rational> coeffsDivisor = {Rational(-1), Rational(1)};
    UnivariatePolynomial<Rational> divisor(coeffsDivisor);

    UnivariatePolynomial<Rational> quotient = dividend / divisor;
    EXPECT_EQ(quotient[0], Rational(1));
    EXPECT_EQ(quotient[1], Rational(1));
    EXPECT_EQ(quotient.degree(), 1);
}

TEST_F(UnivariatePolynomialTest, PolynomialModulo) {
    std::vector<Rational> coeffsDividend = {Rational(2), Rational(3), Rational(1)};
    UnivariatePolynomial<Rational> dividend(coeffsDividend);

    std::vector<Rational> coeffsDivisor = {Rational(1), Rational(1)};
    UnivariatePolynomial<Rational> divisor(coeffsDivisor);

    UnivariatePolynomial<Rational> remainder = dividend % divisor;
    EXPECT_TRUE(remainder.isZero());
}

TEST_F(UnivariatePolynomialTest, DivisionByZeroPolynomial) {
    EXPECT_THROW(p1 / zeroPoly, std::invalid_argument);
}

TEST_F(UnivariatePolynomialTest, UnaryPlus) {
    UnivariatePolynomial<Rational> result = +p1;
    EXPECT_EQ(result, p1);
}

TEST_F(UnivariatePolynomialTest, UnaryMinus) {
    UnivariatePolynomial<Rational> result = -p1;
    EXPECT_EQ(result[0], Rational(-1));
    EXPECT_EQ(result[1], Rational(-2));
    EXPECT_EQ(result[2], Rational(-3));
}

TEST_F(UnivariatePolynomialTest, EqualityOperator) {
    UnivariatePolynomial<Rational> p_copy = p1;
    EXPECT_TRUE(p1 == p_copy);
    EXPECT_FALSE(p1 == p2);
}

TEST_F(UnivariatePolynomialTest, InequalityOperator) {
    UnivariatePolynomial<Rational> p_copy = p1;
    EXPECT_FALSE(p1 != p_copy);
    EXPECT_TRUE(p1 != p2);
}

TEST_F(UnivariatePolynomialTest, IsZero) {
    EXPECT_TRUE(zeroPoly.isZero());
    EXPECT_FALSE(p1.isZero());
}

TEST_F(UnivariatePolynomialTest, IsConstant) {
    EXPECT_TRUE(zeroPoly.isConstant());
    EXPECT_TRUE(onePoly.isConstant());
    EXPECT_FALSE(p1.isConstant());
}

TEST_F(UnivariatePolynomialTest, IsMonic) {
    EXPECT_FALSE(p1.isMonic());
    EXPECT_TRUE(p2.isMonic());
    EXPECT_FALSE(zeroPoly.isMonic());
}

TEST_F(UnivariatePolynomialTest, MakeMonic) {
    UnivariatePolynomial<Rational> monic = p1.makeMonic();
    EXPECT_TRUE(monic.isMonic());
    EXPECT_EQ(monic[0], Rational(1, 3));
    EXPECT_EQ(monic[1], Rational(2, 3));
    EXPECT_EQ(monic[2], Rational(1));
}

TEST_F(UnivariatePolynomialTest, String) {
    EXPECT_EQ(p1.toString(), "3*x² + 2*x + 1");
    EXPECT_EQ(p2.toString(), "x + 2");
    EXPECT_EQ(p3.toString(), "x²");
    EXPECT_EQ(p4.toString(), "1/3*x + 1/2");
}

TEST_F(UnivariatePolynomialTest, Derivative) {
    std::vector<Rational> coeffsq = {Rational(7), Rational(0), Rational(-3),
                                     Rational(8)};
    auto q = UnivariatePolynomial<Rational>(coeffsq);
    EXPECT_EQ(q, p5.derivative());
}