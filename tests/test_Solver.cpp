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

TEST_F(RootFindersTest, SolveSystemInconsistent) {
    auto f1 = x + y - 3'333;
    auto f2 = x + y - 4'444;
    auto _ = solveSystem<Rational>({f1, f2}, findRationalRoots);

    ASSERT_TRUE(std::holds_alternative<std::string>(_));
    EXPECT_EQ(std::get<std::string>(_), "No solution exist in any field extension");
}

TEST_F(RootFindersTest, SolveSystemEmpty) {
    std::vector<MultivariatePolynomial<Rational>> emptySystem;
    auto _ = solveSystem<Rational>(emptySystem, findRationalRoots);

    ASSERT_TRUE(std::holds_alternative<std::string>(_));
    EXPECT_EQ(std::get<std::string>(_), "Empty system is not allowed");
}

TEST_F(RootFindersTest, CharacteristicEquationsInconsistentSystem) {
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

TEST_F(RootFindersTest, CharacteristicEquationsSingleVariable) {
    std::vector<MultivariatePolynomial<Rational>> singleVarSystem;
    singleVarSystem.push_back(3 * (x ^ 99) + 1);

    auto charEqs = characteristicEquations(singleVarSystem);

    EXPECT_EQ(charEqs.size(), 1);
    EXPECT_TRUE(charEqs.find('x') != charEqs.end());
}

TEST_F(RootFindersTest, EdgeCaseConstantPolynomial) {
    MultivariatePolynomial<Rational> constantPoly = Rational(5);

    std::vector<MultivariatePolynomial<Rational>> constantSystem;
    constantSystem.push_back(constantPoly);

    auto _ = solveSystem<Rational>(constantSystem, findRationalRoots);
    ASSERT_TRUE(std::holds_alternative<std::string>(_));

    EXPECT_EQ(std::get<std::string>(_), "No solution exist in any field extension");
}

TEST_F(RootFindersTest, TwoVar1Eq) {
    auto f = x * x + y * y;
    auto solution = solveSystem<Rational>({f}, findRationalRoots);
    auto charEqs = characteristicEquations<Rational>({f});

    ASSERT_TRUE(std::holds_alternative<std::string>(solution));
    EXPECT_EQ(charEqs.size(), 0);
}

TEST_F(RootFindersTest, OneUselessEq) {
    auto f1 = x + y;
    auto f2 = 2 * f1;
    auto f3 = x - y + 77;

    auto _ = solveSystem<Rational>({f1, f2, f3}, findRationalRoots);
    auto solution = std::get<std::vector<std::map<char, Rational>>>(_);
    auto charEqs = characteristicEquations<Rational>({f1, f2, f3});

    EXPECT_EQ(solution.size(), 1);
    EXPECT_TRUE(charEqs.find('x') != charEqs.end());
    EXPECT_TRUE(charEqs.find('y') != charEqs.end());
}

TEST_F(RootFindersTest, CharacteristicEquationsFiniteField7) {
    GaloisField::setPrime(7);
    auto a = defineVariable<GaloisField>('a');
    auto b = defineVariable<GaloisField>('b');
    auto f1 = 3 * a - 2 * b;
    auto f2 = b * b + 5 * a;

    std::map<char, MultivariatePolynomial<GaloisField>> charEqs =
        characteristicEquations(std::vector<MultivariatePolynomial<GaloisField>>{f1, f2});

    EXPECT_EQ(2, charEqs.size());
}

TEST_F(RootFindersTest, OneMoreEqButSolvable) {
    auto f1 = x + y;
    auto f2 = 2 * x + 2 * y;
    auto f3 = x * y + 9;

    auto _ = solveSystem<Rational>({f1, f2, f3}, findRationalRoots);
    auto solution = std::get<std::vector<std::map<char, Rational>>>(_);

    EXPECT_EQ(solution.size(), 2);
}

TEST_F(RootFindersTest, Mod2) {
    GaloisField::setPrime(2);
    auto f1 = a + b - 1;
    auto f2 = a * a + 1;

    auto charEq = characteristicEquations<GaloisField>({f1, f2});

    ASSERT_EQ(charEq.size(), 2);
    EXPECT_TRUE(charEq['a'] == a * a + 1);
    EXPECT_TRUE(charEq['b'] == b * b);
}

TEST_F(RootFindersTest, Mod2Eq) {
    GaloisField::setPrime(2);
    auto f1 = a + 1;
    auto f2 = (a ^ 5) + b;

    auto _ = solveSystem<GaloisField>({f1, f2}, findGaloisFieldRoots);
    auto solution = std::get<std::vector<std::map<char, GaloisField>>>(_);

    EXPECT_EQ(solution[0]['a'], 1);
    EXPECT_EQ(solution[0]['b'], 1);
}

TEST_F(RootFindersTest, PhiCichon) {
    auto f1 = u + v + t - 1;
    auto f2 = (u ^ 2) + (v ^ 2) + (t ^ 2) - 3;
    auto f3 = (u ^ 3) + (v ^ 3) + (t ^ 3) - 4;

    auto _ = solveSystem<Real>({f1, f2, f3}, findRealRoots);
    auto solution = std::get<std::vector<std::map<char, Real>>>(_);
    ASSERT_EQ(solution.size(), 6);

    Real r1(0.0);
    Real r2(-0.618033988749895);
    Real r3(1.61803398874989);

    for (const std::map<char, Real>& sol : solution) {
        std::set<Real> values = {sol.at('u'), sol.at('v'), sol.at('t')};
        std::set<Real> expected = {r1, r2, r3};
        EXPECT_EQ(values, expected);
    }
}

TEST_F(RootFindersTest, IVAPage100) {
    auto f1 = 3 * (X ^ 2) + 2 * Y * Z - 2 * X * T;
    auto f2 = 2 * X * Z - 2 * Y * T;
    auto f3 = 2 * X * Y - 2 * Z - 2 * Z * T;
    auto f4 = (X ^ 2) + (Y ^ 2) + (Z ^ 2) - 1;

    auto _ = solveSystem<BigRational>({f1, f2, f3, f4}, findBigRationalRoots);
    auto solution = std::get<std::vector<std::map<char, BigRational>>>(_);

    std::map<char, BigRational> sol1;
    std::map<char, BigRational> sol2;
    std::map<char, BigRational> sol3;
    std::map<char, BigRational> sol4;
    std::map<char, BigRational> sol5;
    std::map<char, BigRational> sol6;
    std::map<char, BigRational> sol7;
    std::map<char, BigRational> sol8;

    sol1['T'] = BigRational(3, 2);
    sol1['X'] = BigRational(1);
    sol1['Y'] = BigRational(0);
    sol1['Z'] = BigRational(0);

    sol2['T'] = BigRational(-3, 2);
    sol2['X'] = BigRational(-1);
    sol2['Y'] = BigRational(0);
    sol2['Z'] = BigRational(0);

    sol3['T'] = BigRational(0);
    sol3['X'] = BigRational(0);
    sol3['Y'] = BigRational(1);
    sol3['Z'] = BigRational(0);

    sol4['T'] = BigRational(0);
    sol4['X'] = BigRational(0);
    sol4['Y'] = BigRational(-1);
    sol4['Z'] = BigRational(0);

    sol5['T'] = BigRational(-1);
    sol5['X'] = BigRational(0);
    sol5['Y'] = BigRational(0);
    sol5['Z'] = BigRational(1);

    sol6['T'] = BigRational(-1);
    sol6['X'] = BigRational(0);
    sol6['Y'] = BigRational(0);
    sol6['Z'] = BigRational(-1);

    sol7['T'] = BigRational(-4, 3);
    sol7['X'] = BigRational(-2, 3);
    sol7['Y'] = BigRational(1, 3);
    sol7['Z'] = BigRational(2, 3);

    sol8['T'] = BigRational(-4, 3);
    sol8['X'] = BigRational(-2, 3);
    sol8['Y'] = BigRational(-1, 3);
    sol8['Z'] = BigRational(-2, 3);


    ASSERT_EQ(solution.size(), 8);
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol1) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol2) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol3) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol4) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol5) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol6) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol7) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol8) != solution.end());
}

TEST_F(RootFindersTest, TwoAndFive) {
    auto f1 = (X ^ 2) + (Y ^ 2) - 5;
    auto f2 = (X ^ 5) + (Y ^ 5) - 33;

    auto _ = solveSystem<BigRational>({f1, f2}, findBigRationalRoots);
    auto solution = std::get<std::vector<std::map<char, BigRational>>>(_);

    std::map<char, BigRational> sol1;
    std::map<char, BigRational> sol2;

    sol1['X'] = BigRational(1);
    sol1['Y'] = BigRational(2);

    sol2['X'] = BigRational(2);
    sol2['Y'] = BigRational(1);


    ASSERT_EQ(solution.size(), 2);
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol1) != solution.end());
    EXPECT_TRUE(std::find(solution.begin(), solution.end(), sol2) != solution.end());
}
