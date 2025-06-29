#include "BigRational.hpp"
#include "GroebnerBasis.hpp"
#include "MonomialOrders.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"

#include <gtest/gtest.h>
#include <vector>

class GroebnerBasisTest : public ::testing::Test {
protected:
    void SetUp() override {
        x = defineVariable<Rational>('x');
        y = defineVariable<Rational>('y');
        z = defineVariable<Rational>('z');
        u = defineVariable<Rational>('u');
        v = defineVariable<Rational>('v');
        t = defineVariable<Rational>('t');

        X = defineVariable<BigRational>('X');
        Y = defineVariable<BigRational>('Y');
        Z = defineVariable<BigRational>('Z');
        T = defineVariable<BigRational>('T');

        lexXY = std::make_unique<LexOrder>(std::vector<char>{'x', 'y'});
        lexXYZ = std::make_unique<LexOrder>(std::vector<char>{'x', 'y', 'z'});
        gradedLexXY = std::make_unique<GradedLexOrder>(std::vector<char>{'x', 'y'});
        lexTUXYZ = std::make_unique<LexOrder>(std::vector<char>{'t', 'u', 'x', 'y', 'z'});
        lexUVXYZ = std::make_unique<LexOrder>(std::vector<char>{'u', 'v', 'x', 'y', 'z'});

        big_lexTXYZ = std::make_unique<LexOrder>(std::vector<char>{'T', 'X', 'Y', 'Z'});
    }

    MultivariatePolynomial<Rational> x;
    MultivariatePolynomial<Rational> y;
    MultivariatePolynomial<Rational> z;
    MultivariatePolynomial<Rational> u;
    MultivariatePolynomial<Rational> v;
    MultivariatePolynomial<Rational> t;

    MultivariatePolynomial<BigRational> X;
    MultivariatePolynomial<BigRational> Y;
    MultivariatePolynomial<BigRational> Z;
    MultivariatePolynomial<BigRational> T;

    std::unique_ptr<LexOrder> lexXY;
    std::unique_ptr<LexOrder> lexXYZ;
    std::unique_ptr<LexOrder> lexTUXYZ;
    std::unique_ptr<LexOrder> lexUVXYZ;
    std::unique_ptr<GradedLexOrder> gradedLexXY;

    std::unique_ptr<LexOrder> big_lexTXYZ;
};

TEST_F(GroebnerBasisTest, DefineVariable) {
    MultivariatePolynomial<Rational> p({
        {Monomial(std::map<char, int>{{'x', 1}}), 1}
    });
    EXPECT_EQ(p, x);
}

TEST_F(GroebnerBasisTest, polynomialReduce1) {
    auto f = (x ^ 3) + x * (y ^ 2) + 5;
    auto g1 = x * (y ^ 2) - 5;
    auto g2 = (x ^ 2) + 3 * (y ^ 2);
    auto [Q, r] = polynomialReduce(f, {g1, g2}, *lexXY);

    EXPECT_EQ(Q[0], -2);
    EXPECT_EQ(Q[1], x);
    EXPECT_EQ(r, -5);
}

TEST_F(GroebnerBasisTest, polynomialReduce2) {
    auto f = (x ^ 2) * y + 1;
    auto g1 = x * y + 1;
    auto g2 = y + 1;
    auto [Q, r] = polynomialReduce(f, {g1, g2}, *lexXY);

    EXPECT_EQ(Q[0], x);
    EXPECT_EQ(Q[1], 0);
    EXPECT_EQ(r, -x + 1);
}

TEST_F(GroebnerBasisTest, polynomialReduce3) {
    auto f = (x ^ 2) * y + x * (y ^ 2) + (y ^ 2);
    auto g1 = x * y - 1;
    auto g2 = (y ^ 2) - 1;
    auto [Q, r] = polynomialReduce(f, {g1, g2}, *lexXY);

    EXPECT_EQ(Q[0], x + y);
    EXPECT_EQ(Q[1], 1);
    EXPECT_EQ(r, x + y + 1);
}

TEST_F(GroebnerBasisTest, polynomialReduce4) {
    auto f = (x ^ 2) * y + 1;
    auto g1 = y + 1;
    auto g2 = x * y + 1;
    auto [Q, r] = polynomialReduce(f, {g1, g2}, *lexXY);

    EXPECT_EQ(Q[0], x ^ 2);
    EXPECT_EQ(Q[1], 0);
    EXPECT_EQ(r, -(x ^ 2) + 1);
}

TEST_F(GroebnerBasisTest, polynomialReduce5) {
    auto f = (x ^ 5) + (y ^ 5) + (z ^ 5) - 1;
    auto g1 = (x ^ 4) + y + z;
    auto g2 = (x ^ 2) + (y ^ 2) + (z ^ 2);
    auto g3 = x + (y ^ 3) + (z ^ 3);
    auto [Q, r] = polynomialReduce(f, {g1, g2, g3}, *lexXYZ);

    EXPECT_EQ(Q[0], x);
    EXPECT_EQ(Q[1], 0);
    EXPECT_EQ(Q[2], -y - z);
    EXPECT_EQ(r, (y ^ 5) + (z ^ 5) + (y ^ 4) + (z ^ 4) + (y ^ 3) * z + y * (z ^ 3) - 1);
}

TEST_F(GroebnerBasisTest, GroebnerBasis1) {
    auto f1 = (x ^ 3) - 2 * x * y;
    auto f2 = (x ^ 2) * y - 2 * (y ^ 2) + x;
    std::vector<MultivariatePolynomial<Rational>> F = {f1, f2};
    std::vector<MultivariatePolynomial<Rational>> G = calculateGroebnerBasis(F, *gradedLexXY);

    auto g1 = (x ^ 2);
    auto g2 = x * y;
    auto g3 = (y ^ 2) - Rational(1, 2) * x;

    EXPECT_TRUE(std::find(G.begin(), G.end(), g1) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g2) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g3) != G.end());
}

TEST_F(GroebnerBasisTest, GroebnerBasis2) {
    auto f1 = x + y + z - 1;
    auto f2 = (x ^ 2) + (y ^ 2) + (z ^ 2) - 3;
    auto f3 = (x ^ 3) + (y ^ 3) + (z ^ 3) - 4;
    std::vector<MultivariatePolynomial<Rational>> F = {f1, f2, f3};
    std::vector<MultivariatePolynomial<Rational>> G = calculateGroebnerBasis(F, *lexXYZ);

    auto g1 = x + y + z - 1;
    auto g2 = (y ^ 2) + (z ^ 2) + y * z - y - z - 1;
    auto g3 = (z ^ 3) - (z ^ 2) - z;

    EXPECT_TRUE(std::find(G.begin(), G.end(), g1) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g2) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g3) != G.end());
}

TEST_F(GroebnerBasisTest, GroebnerBasis3) {
    auto f1 = 3 * u + 3 * u * v * v - u * u * u - x;
    auto f2 = 3 * v + 3 * u * u * v - v * v * v - y;
    auto f3 = 3 * u * u - 3 * v * v - z;
    std::vector<MultivariatePolynomial<Rational>> F = {f1, f2, f3};
    std::vector<MultivariatePolynomial<Rational>> G = calculateGroebnerBasis(F, *lexUVXYZ);

    auto g = (-Rational(64, 19'683) * (z ^ 9)) + (Rational(16, 243) * (x ^ 2) * (z ^ 6)) -
             (Rational(16, 243) * (y ^ 2) * (z ^ 6)) + (Rational(5, 9) * (x ^ 4) * (z ^ 3)) +
             (Rational(26, 9) * (x ^ 2) * (y ^ 2) * (z ^ 3)) +
             (Rational(16, 9) * (x ^ 2) * (z ^ 5)) + (Rational(5, 9) * (y ^ 4) * (z ^ 3)) +
             (Rational(16, 9) * (y ^ 2) * (z ^ 5)) + (Rational(128, 243) * (z ^ 7)) + (x ^ 6) -
             (3 * (x ^ 4) * (y ^ 2)) + (6 * (x ^ 4) * (z ^ 2)) + (3 * (x ^ 2) * (y ^ 4)) +
             (Rational(80, 9) * (x ^ 2) * (z ^ 4)) - (y ^ 6) - (6 * (y ^ 4) * (z ^ 2)) -
             (Rational(80, 9) * (y ^ 2) * (z ^ 4)) - (3 * (x ^ 4) * z) +
             (6 * (x ^ 2) * (y ^ 2) * z) - (16 * (x ^ 2) * (z ^ 3)) - (3 * (y ^ 4) * z) -
             (16 * (y ^ 2) * (z ^ 3)) - (Rational(64, 3) * (z ^ 5));

    EXPECT_TRUE(std::find(G.begin(), G.end(), g) != G.end());
}

TEST_F(GroebnerBasisTest, GroebnerBasis4) {
    auto f1 = t + u - x;
    auto f2 = (t ^ 2) + 2 * t * u - y;
    auto f3 = (t ^ 3) + 3 * (t ^ 2) * u - z;
    std::vector<MultivariatePolynomial<Rational>> F = {f1, f2, f3};
    std::vector<MultivariatePolynomial<Rational>> G = calculateGroebnerBasis(F, *lexTUXYZ);

    auto g = (x ^ 3) * z - Rational(3, 4) * (x ^ 2) * (y ^ 2) - Rational(3, 2) * x * y * z +
             (y ^ 3) + Rational(1, 4) * (z ^ 2);
             
    EXPECT_EQ(G.size(), 7);
    EXPECT_TRUE(std::find(G.begin(), G.end(), g) != G.end());
}

TEST_F(GroebnerBasisTest, GroebnerBasis5) {
    auto f1 = (z ^ 2) * y + (z ^ 2);
    auto f2 = (x ^ 3) * y + x + y + 1;
    auto f3 = z + (x ^ 2) + (y ^ 3);
    std::vector<MultivariatePolynomial<Rational>> F = {f1, f2, f3};
    std::vector<MultivariatePolynomial<Rational>> G = calculateGroebnerBasis(F, *lexXYZ);

    auto g1 = (z ^ 4) - (z ^ 3);
    auto g2 = (y ^ 11) + 3 * (y ^ 8) * z - 2 * (y ^ 7) - 4 * (y ^ 4) * z + (y ^ 3) + (y ^ 2) +
              2 * y + (z ^ 3) - (z ^ 2) + z + 1;
    auto g3 = (x ^ 2) + (y ^ 3) + z;
    auto g4 = y * (z ^ 2) + (z ^ 2);
    auto g5 = x * y + x + (y ^ 7) + 2 * (y ^ 4) * z - (y ^ 3) - (z ^ 2) - z;
    auto g6 = x * z + (y ^ 10) - (y ^ 9) + (y ^ 8) + 3 * (y ^ 7) * z - (y ^ 7) - 2 * (y ^ 6) * z -
              (y ^ 6) + 2 * (y ^ 5) * z + (y ^ 5) - 2 * (y ^ 4) * z - (y ^ 4) - 2 * (y ^ 3) * z +
              (y ^ 3) + (y ^ 2) * z - y * z + y - (z ^ 3) + 5 * (z ^ 2) + z + 1;

    EXPECT_TRUE(std::find(G.begin(), G.end(), g1) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g2) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g3) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g4) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g5) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g6) != G.end());
}

TEST_F(GroebnerBasisTest, GroebnerBasis6) {
    auto f1 = 3 * (X ^ 2) + 2 * Y * Z - 2 * X * T;
    auto f2 = 2 * X * Z - 2 * Y * T;
    auto f3 = 2 * X * Y - 2 * Z - 2 * Z * T;
    auto f4 = (X ^ 2) + (Y ^ 2) + (Z ^ 2) - 1;
    std::vector<MultivariatePolynomial<BigRational>> F = {f1, f2, f3, f4};
    std::vector<MultivariatePolynomial<BigRational>> G = calculateGroebnerBasis(F, *big_lexTXYZ);

    auto g1 = T - BigRational(3, 2) * X - BigRational(3, 2) * Y * Z -
              BigRational(167'616, 3'835) * (Z ^ 6) + BigRational(36'717, 590) * (Z ^ 4) -
              BigRational(134'419, 7'670) * (Z ^ 2);
    auto g2 = (X ^ 2) + (Y ^ 2) + (Z ^ 2) - BigRational(1, 1);
    auto g3 = X * Y - BigRational(19'584, 3'835) * (Z ^ 5) + BigRational(1'999, 295) * (Z ^ 3) -
              BigRational(6'403, 3'835) * Z;
    auto g4 = X * Z + Y * (Z ^ 2) - BigRational(1'152, 3'835) * (Z ^ 5) -
              BigRational(108, 295) * (Z ^ 3) + BigRational(2'556, 3'835) * Z;
    auto g5 = (Y ^ 3) + Y * (Z ^ 2) - Y - BigRational(9'216, 3'835) * (Z ^ 5) +
              BigRational(906, 295) * (Z ^ 3) - BigRational(2'562, 3'835) * Z;
    auto g6 = (Y ^ 2) * Z - BigRational(6'912, 3'835) * (Z ^ 5) + BigRational(827, 295) * (Z ^ 3) -
              BigRational(3'839, 3'835) * Z;
    auto g7 = Y * (Z ^ 3) - Y * Z - BigRational(576, 59) * (Z ^ 6) +
              BigRational(1'605, 118) * (Z ^ 4) - BigRational(453, 118) * (Z ^ 2);
    auto g8 = (Z ^ 7) - BigRational(1'763, 1'152) * (Z ^ 5) + BigRational(655, 1'152) * (Z ^ 3) -
              BigRational(11, 288) * Z;

    EXPECT_TRUE(std::find(G.begin(), G.end(), g1) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g2) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g3) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g4) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g5) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g6) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g7) != G.end());
    EXPECT_TRUE(std::find(G.begin(), G.end(), g8) != G.end());
}