#include "GroebnerBasis.hpp"
#include "MonomialOrders.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"

#include <gtest/gtest.h>
#include <vector>

class GroebnerBasisTest : public ::testing::Test {
protected:
    void SetUp() override {
        //  Single variables
        x = defineVariable<Rational>('x');
        y = defineVariable<Rational>('y');
        z = defineVariable<Rational>('z');
        u = defineVariable<Rational>('u');
        v = defineVariable<Rational>('v');
        t = defineVariable<Rational>('t');

        //  Common monomial orders
        lexXY = std::make_unique<LexOrder>(std::vector<char>{'x', 'y'});
        lexXYZ = std::make_unique<LexOrder>(std::vector<char>{'x', 'y', 'z'});
        gradedLexXY = std::make_unique<GradedLexOrder>(std::vector<char>{'x', 'y'});
        lexTUXYZ = std::make_unique<LexOrder>(std::vector<char>{'t', 'u', 'x', 'y', 'z'});
        lexUVXYZ = std::make_unique<LexOrder>(std::vector<char>{'u', 'v', 'x', 'y', 'z'});
    }

    //  Variables
    MultivariatePolynomial<Rational> x;
    MultivariatePolynomial<Rational> y;
    MultivariatePolynomial<Rational> z;
    MultivariatePolynomial<Rational> u;
    MultivariatePolynomial<Rational> v;
    MultivariatePolynomial<Rational> t;

    //  Monomial orders
    std::unique_ptr<LexOrder> lexXY;
    std::unique_ptr<LexOrder> lexXYZ;
    std::unique_ptr<LexOrder> lexTUXYZ;
    std::unique_ptr<LexOrder> lexUVXYZ;
    std::unique_ptr<GradedLexOrder> gradedLexXY;
};

TEST_F(GroebnerBasisTest, DefineVariable) {
    MultivariatePolynomial<Rational> p({
        {Monomial(std::map<char, int>{{'x', 1}}), 1}
    });
    EXPECT_EQ(p, x);
}

TEST_F(GroebnerBasisTest, polynomialReduce1) {
    MultivariatePolynomial<Rational> f = (x ^ 3) + x * (y ^ 2) + 5;
    MultivariatePolynomial<Rational> g1 = x * (y ^ 2) - 5;
    MultivariatePolynomial<Rational> g2 = (x ^ 2) + 3 * (y ^ 2);
    std::vector<MultivariatePolynomial<Rational>> G = {g1, g2};
    auto [Q, r] = polynomialReduce(f, G, *lexXY);
    EXPECT_EQ(Q[0], -2);
    EXPECT_EQ(Q[1], x);
    EXPECT_EQ(r, -5);
}

TEST_F(GroebnerBasisTest, polynomialReduce2) {
    MultivariatePolynomial<Rational> f = (x ^ 2) * y + 1;
    MultivariatePolynomial<Rational> g1 = x * y + 1;
    MultivariatePolynomial<Rational> g2 = y + 1;
    std::vector<MultivariatePolynomial<Rational>> G = {g1, g2};
    auto [Q, r] = polynomialReduce(f, G, *lexXY);
    EXPECT_EQ(Q[0], x);
    EXPECT_EQ(Q[1], 0);
    EXPECT_EQ(r, -x + 1);
}

TEST_F(GroebnerBasisTest, polynomialReduce3) {
    MultivariatePolynomial<Rational> f = (x ^ 2) * y + x * (y ^ 2) + (y ^ 2);
    MultivariatePolynomial<Rational> g1 = x * y - 1;
    MultivariatePolynomial<Rational> g2 = (y ^ 2) - 1;
    std::vector<MultivariatePolynomial<Rational>> G = {g1, g2};
    auto [Q, r] = polynomialReduce(f, G, *lexXY);
    EXPECT_EQ(Q[0], x + y);
    EXPECT_EQ(Q[1], 1);
    EXPECT_EQ(r, x + y + 1);
}

TEST_F(GroebnerBasisTest, polynomialReduce4) {
    MultivariatePolynomial<Rational> f = (x ^ 2) * y + 1;
    MultivariatePolynomial<Rational> g1 = y + 1;
    MultivariatePolynomial<Rational> g2 = x * y + 1;
    std::vector<MultivariatePolynomial<Rational>> G = {g1, g2};
    auto [Q, r] = polynomialReduce(f, G, *lexXY);
    EXPECT_EQ(Q[0], x ^ 2);
    EXPECT_EQ(Q[1], 0);
    EXPECT_EQ(r, -(x ^ 2) + 1);
}

TEST_F(GroebnerBasisTest, polynomialReduce5) {
    MultivariatePolynomial<Rational> f = (x ^ 5) + (y ^ 5) + (z ^ 5) - 1;
    MultivariatePolynomial<Rational> g1 = (x ^ 4) + y + z;
    MultivariatePolynomial<Rational> g2 = (x ^ 2) + (y ^ 2) + (z ^ 2);
    MultivariatePolynomial<Rational> g3 = x + (y ^ 3) + (z ^ 3);
    std::vector<MultivariatePolynomial<Rational>> G = {g1, g2, g3};
    auto [Q, r] = polynomialReduce(f, G, *lexXYZ);
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
    std::vector<MultivariatePolynomial<Rational>> expectedG = {g1, g2, g3};
    EXPECT_EQ(G, expectedG);
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
    std::vector<MultivariatePolynomial<Rational>> expectedG = {g1, g2, g3};
    EXPECT_EQ(G, expectedG);
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