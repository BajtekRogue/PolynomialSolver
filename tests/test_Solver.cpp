#include "Monomial.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"
#include "Solver.hpp"
#include "UnivariatePolynomial.hpp"

#include <GaloisField.hpp>
#include <gtest/gtest.h>
#include <map>
#include <stdexcept>
#include <vector>

class SolverTest : public ::testing::Test {
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
    }

    MultivariatePolynomial<Rational> x;
    MultivariatePolynomial<Rational> y;
    MultivariatePolynomial<Rational> z;
    MultivariatePolynomial<GaloisField> a;
    MultivariatePolynomial<GaloisField> b;
    MultivariatePolynomial<GaloisField> c;
    MultivariatePolynomial<Real> t;
};

TEST_F(SolverTest, FromMultivariateToUnivariateBasic) {
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

TEST_F(SolverTest, FindRationalRoots) {
    std::vector<Rational> roots;
    std::vector<Rational> coeffs;

    roots = findRationalRoots(fromMultivariateToUnivariate(x - 2));
    ASSERT_EQ(roots.size(), 1);
    EXPECT_EQ(roots[0], Rational(2));

    roots = findRationalRoots(fromMultivariateToUnivariate(x * x + 5 * x + 6));
    ASSERT_EQ(roots.size(), 2);

    //  Sort the roots for consistent testing
    std::sort(roots.begin(), roots.end());
    EXPECT_EQ(roots[0], Rational(-3));
    EXPECT_EQ(roots[1], Rational(-2));

    roots = findRationalRoots(fromMultivariateToUnivariate(x * x + 5));
    EXPECT_TRUE(roots.empty());

    coeffs = {Rational(-1), Rational(2)};
    UnivariatePolynomial<Rational> f(coeffs);

    roots = findRationalRoots(f);
    EXPECT_EQ(roots.size(), 1);
    EXPECT_EQ(roots[0], Rational(1, 2));

    coeffs = {Rational(2), Rational(-3), Rational(0), Rational(1)};
    UnivariatePolynomial<Rational> g(coeffs);

    roots = findRationalRoots(g);
    std::sort(roots.begin(), roots.end());

    ASSERT_EQ(roots.size(), 2);
    bool foundMinusTwo = std::find(roots.begin(), roots.end(), Rational(-2)) != roots.end();
    bool foundOne = std::find(roots.begin(), roots.end(), Rational(1)) != roots.end();
    EXPECT_TRUE(foundMinusTwo);
    EXPECT_TRUE(foundOne);
}

TEST_F(SolverTest, FindRealRoots) {
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

    auto f5 = fromMultivariateToUnivariate(t * (t - 1) * (t + 1) * (t - 2) * (t + 2));
    roots = findRealRoots(f5);
    ASSERT_EQ(roots.size(), 5);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-2)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(-1)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(1)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(2)) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), Real(0)) != roots.end());
}

TEST_F(SolverTest, SolveSystemInconsistent) {
    auto f1 = x + y - 3333;
    auto f2 = x + y - 4444;
    auto X = solveSystem<Rational>({f1, f2}, findRationalRoots);
    ASSERT_TRUE(std::holds_alternative<std::string>(X));
    EXPECT_EQ(std::get<std::string>(X), "No solution exist in any field extension");
}

TEST_F(SolverTest, SolveSystemEmpty) {
    std::vector<MultivariatePolynomial<Rational>> emptySystem;
    auto X = solveSystem<Rational>(emptySystem, findRationalRoots);
    ASSERT_TRUE(std::holds_alternative<std::string>(X));
    EXPECT_EQ(std::get<std::string>(X), "Empty system is not allowed");
}

TEST_F(SolverTest, CharacteristicEquationsInconsistentSystem) {
    auto f1 = x + y - 1;
    auto f2 = x + y - 11;
    std::map<char, MultivariatePolynomial<Rational>> charEqs1 =
        characteristicEquations<Rational>({f1, f2});
    EXPECT_TRUE(charEqs1.empty());

    auto g1 = x - 1;
    auto g2 = x - 11;
    std::map<char, MultivariatePolynomial<Rational>> charEqs2 =
        characteristicEquations<Rational>({g1, g2});
    EXPECT_TRUE(charEqs2.empty());
}

TEST_F(SolverTest, CharacteristicEquationsSingleVariable) {
    std::vector<MultivariatePolynomial<Rational>> singleVarSystem;
    singleVarSystem.push_back(3 * (x ^ 99) + 1);

    auto charEqs = characteristicEquations(singleVarSystem);

    EXPECT_EQ(charEqs.size(), 1);
    EXPECT_TRUE(charEqs.find('x') != charEqs.end());
}

TEST_F(SolverTest, EdgeCaseConstantPolynomial) {
    std::map<Monomial, Rational> constantTerms;
    constantTerms[Monomial()] = Rational(5);
    MultivariatePolynomial<Rational> constantPoly(constantTerms);

    std::vector<MultivariatePolynomial<Rational>> constantSystem;
    constantSystem.push_back(constantPoly);

    auto X = solveSystem<Rational>(constantSystem, findRationalRoots);
    ASSERT_TRUE(std::holds_alternative<std::string>(X));
    EXPECT_EQ(std::get<std::string>(X), "No solution exist in any field extension");
}

TEST_F(SolverTest, TwoVar1Eq) {
    auto f = x * x + y * y;
    auto solution = solveSystem<Rational>({f}, findRationalRoots);
    auto charEqs = characteristicEquations<Rational>({f});
    ASSERT_TRUE(std::holds_alternative<std::string>(solution));
    EXPECT_EQ(charEqs.size(), 0);
}

TEST_F(SolverTest, OneUselessEq) {
    auto f1 = x + y;
    auto f2 = Rational(2) * f1;
    auto f3 = x - y + Rational(77);

    auto X = solveSystem<Rational>({f1, f2, f3}, findRationalRoots);
    auto solution = std::get<std::vector<std::map<char, Rational>>>(X);
    auto charEqs = characteristicEquations<Rational>({f1, f2, f3});

    EXPECT_EQ(solution.size(), 1);
    EXPECT_TRUE(charEqs.find('x') != charEqs.end());
    EXPECT_TRUE(charEqs.find('y') != charEqs.end());
}

TEST_F(SolverTest, CharacteristicEquationsFiniteField7) {

    GaloisField::setPrime(7);
    auto a = defineVariable<GaloisField>('a');
    auto b = defineVariable<GaloisField>('b');
    auto f1 = 3 * a - 2 * b;
    auto f2 = b * b + 5 * a;

    std::map<char, MultivariatePolynomial<GaloisField>> charEqs =
        characteristicEquations(std::vector<MultivariatePolynomial<GaloisField>>{f1, f2});

    EXPECT_EQ(2, charEqs.size());
}

TEST_F(SolverTest, OneMoreEqButSolvable) {
    auto f1 = x + y;
    auto f2 = 2 * x + 2 * y;
    auto f3 = x * y + 9;

    auto X = solveSystem<Rational>({f1, f2, f3}, findRationalRoots);
    auto solution = std::get<std::vector<std::map<char, Rational>>>(X);

    EXPECT_EQ(solution.size(), 2);
}

TEST_F(SolverTest, Mod2) {
    GaloisField::setPrime(2);
    auto f1 = a + b - 1;
    auto f2 = a * a + 1;

    auto charEq = characteristicEquations<GaloisField>({f1, f2});
    EXPECT_EQ(charEq.size(), 2);
    EXPECT_TRUE(charEq['a'] == a * a + 1);
    EXPECT_TRUE(charEq['b'] == b * b);
}

TEST_F(SolverTest, Mod2Eq) {
    GaloisField::setPrime(2);
    auto f1 = a + 1;
    auto f2 = (a ^ 5) + b;

    auto X = solveSystem<GaloisField>({f1, f2}, findGaloisFieldRoots);
    auto solution = std::get<std::vector<std::map<char, GaloisField>>>(X);

    EXPECT_EQ(solution[0]['a'], 1);
    EXPECT_EQ(solution[0]['b'], 1);
}

TEST_F(SolverTest, PhiCichon) {
    auto f1 = x + y + z - 1;
    auto f2 = x * x + y * y + z * z - 3;
    auto f3 = x * x * x + y * y * y + z * z * z - 4;

    auto X = solveSystem<Rational>({f1, f2, f3}, findRationalRoots);
    auto solution = std::get<std::string>(X);
    EXPECT_EQ(solution, "No solutions found");

    // x + y + z - 1 
    // x ^ 2 + y ^ 2 + z ^ 2 - 3 
    // x ^ 3 + y ^ 3 + z ^ 3 - 4
}