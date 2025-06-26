#include "Rational.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

class RationalTest : public ::testing::Test {
protected:
    void SetUp() override {
        zero = Rational(0, 1);
        one = Rational(1, 1);
        half = Rational(1, 2);
        negHalf = Rational(-1, 2);
        twoThirds = Rational(2, 3);
        threeHalves = Rational(3, 2);
    }

    Rational zero;
    Rational one;
    Rational half;
    Rational negHalf;
    Rational twoThirds;
    Rational threeHalves;
};

TEST_F(RationalTest, DefaultConstructor) {
    Rational r;
    EXPECT_EQ(r.getNumerator(), 0);
    EXPECT_EQ(r.getDenominator(), 1);
    EXPECT_TRUE(r.isZero());
}

TEST_F(RationalTest, SingleArgumentConstructor) {
    Rational r(5);
    EXPECT_EQ(r.getNumerator(), 5);
    EXPECT_EQ(r.getDenominator(), 1);
}

TEST_F(RationalTest, TwoArgumentConstructor) {
    Rational r(3, 4);
    EXPECT_EQ(r.getNumerator(), 3);
    EXPECT_EQ(r.getDenominator(), 4);
}

TEST_F(RationalTest, ConstructorSimplification) {
    Rational r(6, 9); 
    EXPECT_EQ(r.getNumerator(), 2);
    EXPECT_EQ(r.getDenominator(), 3);
}

TEST_F(RationalTest, ConstructorNegativeDenominator) {
    Rational r(3, -4); 
    EXPECT_EQ(r.getNumerator(), -3);
    EXPECT_EQ(r.getDenominator(), 4);
}

TEST_F(RationalTest, ConstructorZeroDenominator) {
    EXPECT_THROW(Rational(1, 0), std::invalid_argument);
}

TEST_F(RationalTest, ConstructorZeroNumerator) {
    Rational r(0, 5);
    EXPECT_EQ(r.getNumerator(), 0);
    EXPECT_EQ(r.getDenominator(), 1); 
}

TEST_F(RationalTest, AdditionBasic) {
    Rational result = half + twoThirds;
    EXPECT_EQ(result.getNumerator(), 7);
    EXPECT_EQ(result.getDenominator(), 6);
}

TEST_F(RationalTest, AdditionWithZero) {
    Rational result = half + zero;
    EXPECT_EQ(result, half);
}

TEST_F(RationalTest, AdditionWithInteger) {
    Rational result = half + 2;
    EXPECT_EQ(result.getNumerator(), 5);
    EXPECT_EQ(result.getDenominator(), 2);
}

TEST_F(RationalTest, AdditionCommutative) {
    EXPECT_EQ(half + twoThirds, twoThirds + half);
    EXPECT_EQ(3 + half, half + 3);
}

TEST_F(RationalTest, SubtractionBasic) {
    Rational result = twoThirds - half;
    EXPECT_EQ(result.getNumerator(), 1);
    EXPECT_EQ(result.getDenominator(), 6);
}

TEST_F(RationalTest, SubtractionWithSelf) {
    Rational result = half - half;
    EXPECT_TRUE(result.isZero());
}

TEST_F(RationalTest, SubtractionWithInteger) {
    Rational result = threeHalves - 1;
    EXPECT_EQ(result, half);
}

TEST_F(RationalTest, MultiplicationBasic) {
    Rational result = half * twoThirds;
    EXPECT_EQ(result.getNumerator(), 1);
    EXPECT_EQ(result.getDenominator(), 3);
}

TEST_F(RationalTest, MultiplicationWithZero) {
    Rational result = half * zero;
    EXPECT_TRUE(result.isZero());
}

TEST_F(RationalTest, MultiplicationWithOne) {
    Rational result = half * one;
    EXPECT_EQ(result, half);
}

TEST_F(RationalTest, MultiplicationWithInteger) {
    Rational result = half * 3;
    EXPECT_EQ(result, threeHalves);
}

TEST_F(RationalTest, MultiplicationCommutative) {
    EXPECT_EQ(half * twoThirds, twoThirds * half);
    EXPECT_EQ(2 * half, half * 2);
}

TEST_F(RationalTest, DivisionBasic) {
    Rational result = half / twoThirds;
    EXPECT_EQ(result.getNumerator(), 3);
    EXPECT_EQ(result.getDenominator(), 4);
}

TEST_F(RationalTest, DivisionByOne) {
    Rational result = half / one;
    EXPECT_EQ(result, half);
}

TEST_F(RationalTest, DivisionBySelf) {
    Rational result = half / half;
    EXPECT_EQ(result, one);
}

TEST_F(RationalTest, DivisionByZero) {
    EXPECT_THROW(half / zero, std::invalid_argument);
}

TEST_F(RationalTest, DivisionWithInteger) {
    Rational result = half / 2;
    EXPECT_EQ(result.getNumerator(), 1);
    EXPECT_EQ(result.getDenominator(), 4);
}

TEST_F(RationalTest, UnaryPlus) {
    Rational result = +half;
    EXPECT_EQ(result, half);
}

TEST_F(RationalTest, UnaryMinus) {
    Rational result = -half;
    EXPECT_EQ(result, negHalf);
}

TEST_F(RationalTest, DoubleNegative) {
    Rational result = -(-half);
    EXPECT_EQ(result, half);
}

TEST_F(RationalTest, EqualityOperator) {
    EXPECT_TRUE(half == half);
    EXPECT_TRUE(Rational(1, 2) == Rational(2, 4));
    EXPECT_FALSE(half == twoThirds);
}

TEST_F(RationalTest, InequalityOperator) {
    EXPECT_FALSE(half != half);
    EXPECT_TRUE(half != twoThirds);
}

TEST_F(RationalTest, LessThanOperator) {
    EXPECT_TRUE(half < twoThirds);
    EXPECT_FALSE(twoThirds < half);
    EXPECT_FALSE(half < half);
}

TEST_F(RationalTest, GreaterThanOperator) {
    EXPECT_TRUE(twoThirds > half);
    EXPECT_FALSE(half > twoThirds);
    EXPECT_FALSE(half > half);
}

TEST_F(RationalTest, LessEqualOperator) {
    EXPECT_TRUE(half <= twoThirds);
    EXPECT_TRUE(half <= half);
    EXPECT_FALSE(twoThirds <= half);
}

TEST_F(RationalTest, GreaterEqualOperator) {
    EXPECT_TRUE(twoThirds >= half);
    EXPECT_TRUE(half >= half);
    EXPECT_FALSE(half >= twoThirds);
}

TEST_F(RationalTest, PlusEquals) {
    Rational r = half;
    r += twoThirds;
    EXPECT_EQ(r.getNumerator(), 7);
    EXPECT_EQ(r.getDenominator(), 6);
}

TEST_F(RationalTest, MinusEquals) {
    Rational r = twoThirds;
    r -= half;
    EXPECT_EQ(r.getNumerator(), 1);
    EXPECT_EQ(r.getDenominator(), 6);
}

TEST_F(RationalTest, TimesEquals) {
    Rational r = half;
    r *= twoThirds;
    EXPECT_EQ(r.getNumerator(), 1);
    EXPECT_EQ(r.getDenominator(), 3);
}

TEST_F(RationalTest, DivideEquals) {
    Rational r = half;
    r /= twoThirds;
    EXPECT_EQ(r.getNumerator(), 3);
    EXPECT_EQ(r.getDenominator(), 4);
}


TEST_F(RationalTest, IsZero) {
    EXPECT_TRUE(zero.isZero());
    EXPECT_FALSE(half.isZero());
}

TEST_F(RationalTest, IsOne) {
    EXPECT_TRUE(one.isOne());
    EXPECT_FALSE(half.isOne());
}

TEST_F(RationalTest, IsInteger) {
    EXPECT_TRUE(one.isInteger());
    EXPECT_TRUE(zero.isInteger());
    EXPECT_FALSE(half.isInteger());
}

TEST_F(RationalTest, AbsoluteValue) {
    EXPECT_EQ(negHalf.abs(), half);
    EXPECT_EQ(half.abs(), half);
    EXPECT_EQ(zero.abs(), zero);
}

TEST_F(RationalTest, Reciprocal) {
    Rational result = half.reciprocal();
    EXPECT_EQ(result.getNumerator(), 2);
    EXPECT_EQ(result.getDenominator(), 1);
    EXPECT_THROW(zero.reciprocal(), std::invalid_argument);
}

TEST_F(RationalTest, MultiplicativeInverse) {
    Rational result = half.multiplicativeInverse();
    EXPECT_EQ(result.getNumerator(), 2);
    EXPECT_EQ(result.getDenominator(), 1);
    EXPECT_THROW(zero.multiplicativeInverse(), std::invalid_argument);
}

TEST_F(RationalTest, AdditiveInverse) {
    Rational result = half.additiveInverse();
    EXPECT_EQ(result, negHalf);
}

TEST_F(RationalTest, PowerZero) {
    Rational result = half.power(0);
    EXPECT_EQ(result, one);
}

TEST_F(RationalTest, PowerOne) {
    Rational result = half.power(1);
    EXPECT_EQ(result, half);
}

TEST_F(RationalTest, PowerPositive) {
    Rational result = half.power(2);
    EXPECT_EQ(result.getNumerator(), 1);
    EXPECT_EQ(result.getDenominator(), 4);
}

TEST_F(RationalTest, PowerNegative) {
    Rational result = half.power(-1);
    EXPECT_EQ(result.getNumerator(), 2);
    EXPECT_EQ(result.getDenominator(), 1);
}

TEST_F(RationalTest, PowerNegativeSquare) {
    Rational result = half.power(-2);
    EXPECT_EQ(result.getNumerator(), 4);
    EXPECT_EQ(result.getDenominator(), 1);
}

TEST_F(RationalTest, ToDouble) {
    EXPECT_DOUBLE_EQ(half.toDouble(), 0.5);
    EXPECT_DOUBLE_EQ(twoThirds.toDouble(), 2.0 / 3.0);
}

TEST_F(RationalTest, ToFloat) {
    EXPECT_FLOAT_EQ(half.toFloat(), 0.5f);
    EXPECT_FLOAT_EQ(twoThirds.toFloat(), 2.0f / 3.0f);
}

TEST_F(RationalTest, ToStringInteger) {
    EXPECT_EQ(one.toString(), "1");
    EXPECT_EQ(zero.toString(), "0");
}

TEST_F(RationalTest, ToStringFraction) {
    EXPECT_EQ(half.toString(), "1/2");
    EXPECT_EQ(twoThirds.toString(), "2/3");
}

TEST_F(RationalTest, ToStringNegative) {
    EXPECT_EQ(negHalf.toString(), "-1/2");
}

TEST_F(RationalTest, StaticZero) {
    Rational z = Rational::zero();
    EXPECT_TRUE(z.isZero());
}

TEST_F(RationalTest, StaticOne) {
    Rational o = Rational::one();
    EXPECT_TRUE(o.isOne());
}

TEST_F(RationalTest, LargeNumbers) {
    Rational large1(1'000'000, 3);
    Rational large2(2'000'000, 6);
    EXPECT_EQ(large1, large2);
}

TEST_F(RationalTest, OverflowProtection) {
    int64_t large = 1'000'000'000LL;
    Rational r1(large, 1);
    Rational r2(1, large);
    Rational product = r1 * r2;
    EXPECT_EQ(product, one);
}

TEST_F(RationalTest, SimplificationWithNegatives) {
    Rational r1(-6, -9); 
    EXPECT_EQ(r1.getNumerator(), 2);
    EXPECT_EQ(r1.getDenominator(), 3);
    Rational r2(-6, 9); 
    EXPECT_EQ(r2.getNumerator(), -2);
    EXPECT_EQ(r2.getDenominator(), 3);
}