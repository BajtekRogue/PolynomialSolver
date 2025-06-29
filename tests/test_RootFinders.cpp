#include "BigRational.hpp"
#include "GaloisField.hpp"
#include "Monomial.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"
#include "Solver.hpp"
#include "UnivariatePolynomial.hpp"

#include <gtest/gtest.h>
#include <map>
#include <stdexcept>
#include <vector>

class RootFindersTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        GaloisField::setPrime(7);
    }

    void SetUp() override {
        x = defineVariable<Rational>('x');
        y = defineVariable<Rational>('y');
        z = defineVariable<Rational>('z');

        a = defineVariable<GaloisField>('a');
        b = defineVariable<GaloisField>('b');
        c = defineVariable<GaloisField>('c');

        t = defineVariable<Real>('t');
        u = defineVariable<Real>('u');
        v = defineVariable<Real>('v');

        X = defineVariable<BigRational>('X');
        Y = defineVariable<BigRational>('Y');
        Z = defineVariable<BigRational>('Z');
        T = defineVariable<BigRational>('T');
    }

    MultivariatePolynomial<Rational> x;
    MultivariatePolynomial<Rational> y;
    MultivariatePolynomial<Rational> z;

    MultivariatePolynomial<GaloisField> a;
    MultivariatePolynomial<GaloisField> b;
    MultivariatePolynomial<GaloisField> c;

    MultivariatePolynomial<Real> t;
    MultivariatePolynomial<Real> u;
    MultivariatePolynomial<Real> v;

    MultivariatePolynomial<BigRational> X;
    MultivariatePolynomial<BigRational> Y;
    MultivariatePolynomial<BigRational> Z;
    MultivariatePolynomial<BigRational> T;
};

TEST_F(RootFindersTest, FromMultivariateToUnivariateBasic) {
    auto f1 = fromMultivariateToUnivariate(x - 2);
    EXPECT_EQ(f1[0], Rational(-2));
    EXPECT_EQ(f1[1], Rational(1));
    EXPECT_EQ(f1.degree(), 1);

    auto f2 = fromMultivariateToUnivariate(3 * x * x - 2 * x + 1);
    EXPECT_EQ(f2[0], Rational(1));
    EXPECT_EQ(f2[1], Rational(-2));
    EXPECT_EQ(f2[2], Rational(3));
    EXPECT_EQ(f2.degree(), 2);

    EXPECT_THROW(fromMultivariateToUnivariate(x * y), std::runtime_error);
}

TEST_F(RootFindersTest, FindRationalRoots) {
    std::vector<Rational> roots;
    std::vector<Rational> coeffs;

    roots = findRationalRoots(fromMultivariateToUnivariate(x - 2));
    ASSERT_EQ(roots.size(), 1);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(2)) != roots.end());

    roots = findRationalRoots(fromMultivariateToUnivariate(x * x + 5 * x + 6));
    ASSERT_EQ(roots.size(), 2);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(-2)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(-3)) != roots.end());

    roots = findRationalRoots(fromMultivariateToUnivariate(x * x + 5));
    EXPECT_TRUE(roots.empty());

    roots = findRationalRoots(fromMultivariateToUnivariate(x * x * x + 7 * x * x - 8 * x));
    ASSERT_EQ(roots.size(), 3);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(0)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(1)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(-8)) != roots.end());


    coeffs = {Rational(-1), Rational(2)};
    UnivariatePolynomial<Rational> f(coeffs);

    roots = findRationalRoots(f);
    ASSERT_EQ(roots.size(), 1);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(1, 2)) != roots.end());

    coeffs = {Rational(2), Rational(-3), Rational(0), Rational(1)};
    UnivariatePolynomial<Rational> g(coeffs);

    roots = findRationalRoots(g);
    std::sort(roots.begin(), roots.end());

    ASSERT_EQ(roots.size(), 2);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(1)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Rational(-2)) != roots.end());

    std::vector<BigRational> big_roots = findBigRationalRoots(
        fromMultivariateToUnivariate((Z ^ 7) - BigRational(1'763, 1'152) * (Z ^ 5) +
                                     BigRational(655, 1'152) * (Z ^ 3) - BigRational(11, 288) * Z));

    EXPECT_TRUE(std::find(big_roots.begin(), big_roots.end(), BigRational(0)) != big_roots.end());
    EXPECT_TRUE(std::find(big_roots.begin(), big_roots.end(), BigRational(1)) != big_roots.end());
    EXPECT_TRUE(std::find(big_roots.begin(), big_roots.end(), BigRational(-1)) != big_roots.end());
    EXPECT_TRUE(std::find(big_roots.begin(), big_roots.end(), BigRational(2, 3)) !=
                big_roots.end());
    EXPECT_TRUE(std::find(big_roots.begin(), big_roots.end(), BigRational(-2, 3)) !=
                big_roots.end());
}

TEST_F(RootFindersTest, FindRealRoots) {
    std::vector<Real> roots;

    auto f1 = fromMultivariateToUnivariate(3 * t + 11);
    roots = findRealRoots(f1);

    ASSERT_EQ(roots.size(), 1);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-11 / static_cast<double>(3))) !=
                roots.end());

    auto f2 = fromMultivariateToUnivariate(1 + t + (t ^ 5));
    roots = findRealRoots(f2);

    ASSERT_EQ(roots.size(), 1);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-0.754877666246693)) != roots.end());

    auto f3 = fromMultivariateToUnivariate((t ^ 3) + 4 * (t ^ 2) - 11 * t - 2);
    roots = findRealRoots(f3);

    ASSERT_EQ(roots.size(), 3);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(2)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-5.82842712474619)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-0.171572875253810)) != roots.end());

    auto f4 = fromMultivariateToUnivariate((t ^ 5) - 6 * (t ^ 4) - 133 * (t ^ 3) + 610 * (t ^ 2) +
                                           3'084 * t - 3'520);
    roots = findRealRoots(f4);

    Real::setEpsilon(0.1);
    ASSERT_EQ(roots.size(), 5);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-10)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-4)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(1)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(8)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(11)) != roots.end());
    Real::setEpsilon(1e-7);

    auto f5 = fromMultivariateToUnivariate(t * (t - 1) * (t + 1) * (t - 2) * (t + 2));
    roots = findRealRoots(f5);

    ASSERT_EQ(roots.size(), 5);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-2)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-1)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(1)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(2)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(0)) != roots.end());
}

