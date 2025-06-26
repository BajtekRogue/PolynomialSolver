#include "Monomial.hpp"

#include <gtest/gtest.h>

class MonomialTest : public ::testing::Test {
protected:
    void SetUp() override {
        //  Common monomials used across multiple tests
        defaultMonomial = Monomial();

        //  m1: x²y³z⁴ (degree 9, 3 variables)
        m1 = Monomial({
            {'x', 2},
            {'y', 3},
            {'z', 4}
        });

        //  m2: x³y⁴z¹⁰ (degree 17, 3 variables) - for comparison/multiplication tests
        m2 = Monomial({
            {'x', 3 },
            {'y', 4 },
            {'z', 10}
        });

        //  m3: Same as m1 but different order - for equality test
        m3 = Monomial({
            {'z', 4},
            {'y', 3},
            {'x', 2}
        });

        //  m4: a²b³c⁴ - different variables for inequality test
        m4 = Monomial({
            {'a', 2},
            {'b', 3},
            {'c', 4}
        });

        //  m5: x²b⁴ with zero exponent 'a' - for zero removal test
        m5 = Monomial({
            {'x', 2},
            {'a', 0},
            {'b', 4}
        });

        //  m6: a³yb¹ - different variables with overlap for multiplication
        m6 = Monomial({
            {'a', 3},
            {'y', 4},
            {'b', 1}
        });

        //  m7: w⁴x²y³z¹ - for toString test
        m7 = Monomial({
            {'x', 2},
            {'y', 3},
            {'z', 1},
            {'w', 4}
        });

        //  m8: a³b⁴c¹⁰d⁴ - for LCM test with no common variables
        m8 = Monomial({
            {'a', 3 },
            {'b', 4 },
            {'c', 10},
            {'d', 4 }
        });
    }

    void TearDown() override {
        //  Cleanup if needed
    }

    //  Test fixture members - reusable monomials
    Monomial defaultMonomial;
    Monomial m1; //  x²y³z⁴
    Monomial m2; //  x³y⁴z¹⁰
    Monomial m3; //  z⁴y³x² (same as m1, different order)
    Monomial m4; //  a²b³c⁴
    Monomial m5; //  x²b⁴ (with zero 'a' removed)
    Monomial m6; //  a³y⁴b¹
    Monomial m7; //  w⁴x²y³z¹
    Monomial m8; //  a³b⁴c¹⁰d⁴
};

TEST_F(MonomialTest, DefaultConstructor) {
    EXPECT_EQ(defaultMonomial.getDegree(), 0);
    EXPECT_EQ(defaultMonomial.getNumVariables(), 0);
    EXPECT_EQ(defaultMonomial.getVariables(), std::vector<char>{});
}

TEST_F(MonomialTest, NormalConstructor) {
    EXPECT_EQ(m1.getDegree(), 9);
    EXPECT_EQ(m1.getNumVariables(), 3);
    EXPECT_EQ(m1.getVariables(), (std::vector<char>{'x', 'y', 'z'}));
}

TEST_F(MonomialTest, RemoveZeroExponentsDuringConstructor) {
    EXPECT_EQ(m5.getDegree(), 6);
    EXPECT_EQ(m5.getNumVariables(), 2);
    EXPECT_EQ(m5.getVariables(), (std::vector<char>{'b', 'x'}));
}

TEST_F(MonomialTest, ThrowErrorNegativeExponentsDuringConstructor) {
    EXPECT_THROW(Monomial({
                     {'x', 2 },
                     {'a', -1},
                     {'b', 4 }
    }),
                 std::invalid_argument);
}

TEST_F(MonomialTest, CopyConstructor) {
    Monomial n = Monomial(m1);
    EXPECT_EQ(n.getDegree(), 9);
    EXPECT_EQ(n.getNumVariables(), 3);
    EXPECT_EQ(n.getVariables(), (std::vector<char>{'x', 'y', 'z'}));
}

TEST_F(MonomialTest, GetExponent) {
    EXPECT_EQ(m1.getExponent('x'), 2);
    EXPECT_EQ(m1.getExponent('y'), 3);
    EXPECT_EQ(m1.getExponent('z'), 4);
    EXPECT_EQ(m1.getExponent('w'), 0);
}

TEST_F(MonomialTest, Equality) {
    EXPECT_EQ(m1, m3); //  Same monomial, different construction order
    EXPECT_NE(m1, m4); //  Different variables
}

TEST_F(MonomialTest, Comparison) {
    EXPECT_GE(m2, m1);
    EXPECT_LE(m1, m2);
    EXPECT_GT(m2, m1);
    EXPECT_LT(m1, m2);
}

TEST_F(MonomialTest, MultiplicationSameVariables) {
    Monomial result = m1 * m2;
    Monomial expected = Monomial({
        {'x', 5 },
        {'y', 7 },
        {'z', 14}
    });
    EXPECT_EQ(result, expected);
}

TEST_F(MonomialTest, MultiplicationDifferentVariables) {
    Monomial result = m1 * m6;
    Monomial expected = Monomial({
        {'x', 2},
        {'y', 7},
        {'z', 4},
        {'a', 3},
        {'b', 1}
    });
    EXPECT_EQ(result, expected);
}

TEST_F(MonomialTest, MultiplicationInPlaceSameVariables) {
    Monomial temp = m1; //  Copy m1
    temp *= m2;
    Monomial expected = Monomial({
        {'x', 5 },
        {'y', 7 },
        {'z', 14}
    });
    EXPECT_EQ(temp, expected);
}

TEST_F(MonomialTest, MultiplicationInPlaceDifferentVariables) {
    Monomial temp = m1; //  Copy m1
    temp *= m6;
    Monomial expected = Monomial({
        {'x', 2},
        {'y', 7},
        {'z', 4},
        {'a', 3},
        {'b', 1}
    });
    EXPECT_EQ(temp, expected);
}

TEST_F(MonomialTest, Division) {
    Monomial result = m2 / m1;
    Monomial expected = Monomial({
        {'x', 1},
        {'y', 1},
        {'z', 6}
    });
    EXPECT_EQ(result, expected);
}

TEST_F(MonomialTest, DivisionErrorHigherExponent) {
    EXPECT_EQ(m1 / m2, Monomial::null);
}

TEST_F(MonomialTest, DivisionErrorDifferentVariables) {
    EXPECT_EQ(m1 / m6, Monomial::null);
}

TEST_F(MonomialTest, DivisionInPlace) {
    Monomial temp = m2; //  Copy m2
    temp /= m1;
    Monomial expected = Monomial({
        {'x', 1},
        {'y', 1},
        {'z', 6}
    });
    EXPECT_EQ(temp, expected);
}

TEST_F(MonomialTest, DivisionInPlaceError) {
    Monomial temp = m6; //  Copy m6
    EXPECT_EQ(temp /= m1, Monomial::null);
}

TEST_F(MonomialTest, ToString) {
    EXPECT_EQ(m7.toString(), "w⁴x²y³z");
}

TEST_F(MonomialTest, LeastCommonMultipleOverlappingVariables) {
    Monomial result = Monomial::lcm(m7, m2);
    Monomial expected = Monomial({
        {'x', 3 },
        {'y', 4 },
        {'z', 10},
        {'w', 4 }
    });
    EXPECT_EQ(result, expected);
}

TEST_F(MonomialTest, LeastCommonMultipleNoOverlappingVariables) {
    Monomial result = Monomial::lcm(m7, m8);
    Monomial expected = Monomial({
        {'x', 2 },
        {'y', 3 },
        {'z', 1 },
        {'w', 4 },
        {'a', 3 },
        {'b', 4 },
        {'c', 10},
        {'d', 4 }
    });
    EXPECT_EQ(result, expected);
}
