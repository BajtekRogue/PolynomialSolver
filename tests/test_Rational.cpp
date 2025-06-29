#include "Rational.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

class RationalTest : public ::testing::Test {
protected:
    void SetUp() override {

    };
};

TEST_F(RationalTest, Constructor) {
    Rational _;
    EXPECT_EQ(_.getNumerator(), 0);
    EXPECT_EQ(_.getDenominator(), 1);

    Rational a(100, 300);
    EXPECT_EQ(a.getNumerator(), 1);
    EXPECT_EQ(a.getDenominator(), 3);

    Rational b(-100, 300);
    EXPECT_EQ(b.getNumerator(), -1);
    EXPECT_EQ(b.getDenominator(), 3);

    Rational c(100, -300);
    EXPECT_EQ(c.getNumerator(), -1);
    EXPECT_EQ(c.getDenominator(), 3);

    Rational d(-100, -300);
    EXPECT_EQ(d.getNumerator(), 1);
    EXPECT_EQ(d.getDenominator(), 3);

    Rational e("1234");
    EXPECT_EQ(e.getNumerator(), 1'234);
    EXPECT_EQ(e.getDenominator(), 1);

    Rational f("-1234");
    EXPECT_EQ(f.getNumerator(), -1'234);
    EXPECT_EQ(f.getDenominator(), 1);

    Rational g("123/91");
    EXPECT_EQ(g.getNumerator(), 123);
    EXPECT_EQ(g.getDenominator(), 91);

    Rational h("-123/91");
    EXPECT_EQ(h.getNumerator(), -123);
    EXPECT_EQ(h.getDenominator(), 91);

    EXPECT_THROW(Rational("345/-31"), std::invalid_argument);
    EXPECT_THROW(Rational("345a/31"), std::invalid_argument);
    EXPECT_THROW(Rational("345/b31"), std::invalid_argument);
    EXPECT_THROW(Rational("345/0"), std::invalid_argument);
    EXPECT_THROW(Rational(3, 0), std::invalid_argument);
}

TEST_F(RationalTest, EqualityAndComparison) {
    Rational a(1, 2);
    Rational b(-10, 9);
    Rational c(3);
    Rational d(-10);
    Rational e(1, 2);
    Rational f(7, 4);
    Rational g(-100, -200);
    Rational h(60, 20);

    EXPECT_EQ(a, g);
    EXPECT_EQ(c, h);

    EXPECT_LT(b, a);
    EXPECT_LT(a, c);
    EXPECT_LT(d, a);
    EXPECT_LE(a, e);
    EXPECT_LT(a, f);
    EXPECT_LE(g, a);
    EXPECT_LE(a, h);
}

TEST_F(RationalTest, FieldArithmetic) {
    Rational a(1, 2);
    Rational b(-10, 9);
    Rational c(3);
    Rational d(-10);
    Rational e(1, 2);
    Rational f(7, 4);

    EXPECT_EQ(a + b, Rational(-11, 18));
    EXPECT_EQ(f + 11, Rational(51, 4));

    EXPECT_EQ(c - e, Rational(5, 2));
    EXPECT_EQ(f - 1, Rational(3, 4));

    EXPECT_EQ(a * b, Rational(-5, 9));
    EXPECT_EQ(a * c, Rational(3, 2));

    EXPECT_EQ(a / b, Rational(-9, 20));
    EXPECT_EQ(f / c, Rational(7, 12));
}

TEST_F(RationalTest, CompoundAssignmentOperators) {
    Rational a(1, 2);
    Rational b(1, 3);
    a += b;
    EXPECT_EQ(a, Rational(5, 6));

    Rational c(2, 3);
    c += 2;
    EXPECT_EQ(c, Rational(8, 3));

    Rational d(3, 4);
    Rational e(1, 4);
    d -= e;
    EXPECT_EQ(d, Rational(1, 2));

    Rational f(5, 2);
    f -= 1;
    EXPECT_EQ(f, Rational(3, 2));

    Rational g(2, 3);
    Rational h(3, 5);
    g *= h;
    EXPECT_EQ(g, Rational(2, 5));

    Rational i(3, 7);
    i *= 2;
    EXPECT_EQ(i, Rational(6, 7));

    Rational j(4, 5);
    Rational k(2, 3);
    j /= k;
    EXPECT_EQ(j, Rational(6, 5));

    Rational l(8, 3);
    l /= 4;
    EXPECT_EQ(l, Rational(2, 3));
}

TEST_F(RationalTest, UnaryOperators) {
    Rational a(3, 4);
    Rational b(-2, 5);

    EXPECT_EQ(-a, Rational(-3, 4));
    EXPECT_EQ(-b, Rational(2, 5));
    EXPECT_EQ(-(-a), a);

    EXPECT_EQ(+a, a);
    EXPECT_EQ(+b, b);
}

TEST_F(RationalTest, MixedArithmetic) {
    Rational a(1, 2);
    int b = 3;

    EXPECT_EQ(a + b, Rational(7, 2));
    EXPECT_EQ(b + a, Rational(7, 2));
    EXPECT_EQ(a - b, Rational(-5, 2));
    EXPECT_EQ(b - a, Rational(5, 2));
    EXPECT_EQ(a * b, Rational(3, 2));
    EXPECT_EQ(b * a, Rational(3, 2));
    EXPECT_EQ(a / b, Rational(1, 6));
    EXPECT_EQ(b / a, Rational(6, 1));
}

TEST_F(RationalTest, EdgeCases) {
    Rational zero(0);
    Rational one(1);
    Rational a(3, 4);

    EXPECT_EQ(a + zero, a);
    EXPECT_EQ(zero + a, a);
    EXPECT_EQ(a - zero, a);
    EXPECT_EQ(zero - a, -a);
    EXPECT_EQ(a * zero, zero);
    EXPECT_EQ(zero * a, zero);

    EXPECT_EQ(a * one, a);
    EXPECT_EQ(one * a, a);
    EXPECT_EQ(a / one, a);

    EXPECT_THROW(a / zero, std::invalid_argument);

    Rational b(2, 3);
    EXPECT_EQ(b - b, zero);
    EXPECT_EQ(b / b, one);
}

TEST_F(RationalTest, PowerOperations) {
    Rational a(2, 3);
    Rational b(-1, 2);

    EXPECT_EQ(a * a, Rational(4, 9));
    EXPECT_EQ(b * b, Rational(1, 4));
    EXPECT_EQ(a * a * a, Rational(8, 27));
}

TEST_F(RationalTest, FractionSimplification) {
    Rational a(24, 36);
    EXPECT_EQ(a, Rational(2, 3));

    Rational b(-15, 25);
    EXPECT_EQ(b, Rational(-3, 5));

    Rational c(100, -150);
    EXPECT_EQ(c, Rational(-2, 3));
}

TEST_F(RationalTest, CompoundAssignmentWithSelf) {
    Rational a(1, 2);
    a += a;
    EXPECT_EQ(a, Rational(1, 1));

    Rational b(3, 4);
    b -= b;
    EXPECT_EQ(b, Rational(0, 1));

    Rational c(2, 5);
    c *= c;
    EXPECT_EQ(c, Rational(4, 25));

    Rational d(7, 3);
    d /= d;
    EXPECT_EQ(d, Rational(1, 1));
}

TEST_F(RationalTest, ChainedOperations) {
    Rational a(1, 2);
    Rational b(1, 3);
    Rational c(1, 6);

    EXPECT_EQ(a + b + c, Rational(1, 1));
    EXPECT_EQ(a * b * c, Rational(1, 36));

    Rational d(2, 3);
    d += Rational(1, 6);
    d *= Rational(6, 5);
    EXPECT_EQ(d, Rational(1, 1));
}