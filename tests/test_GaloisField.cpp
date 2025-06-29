#include "GaloisField.hpp"

#include <gtest/gtest.h>

class GaloisFieldTest : public ::testing::Test {
protected:
    void SetUp() override { }

    void TearDown() override { }
};


TEST_F(GaloisFieldTest, PrimeSettingValid) {
    EXPECT_NO_THROW(GaloisField::setPrime(7));
    EXPECT_EQ(GaloisField::prime, 7);

    EXPECT_NO_THROW(GaloisField::setPrime(11));
    EXPECT_EQ(GaloisField::prime, 11);

    EXPECT_NO_THROW(GaloisField::setPrime(13));
    EXPECT_EQ(GaloisField::prime, 13);
}

TEST_F(GaloisFieldTest, PrimeSettingInvalid) {
    EXPECT_FALSE(GaloisField::setPrime(1));
    EXPECT_FALSE(GaloisField::setPrime(0));
    EXPECT_FALSE(GaloisField::setPrime(-5));
    EXPECT_FALSE(GaloisField::setPrime(4));
    EXPECT_FALSE(GaloisField::setPrime(9));
    EXPECT_FALSE(GaloisField::setPrime(15));
}

TEST_F(GaloisFieldTest, ConstructorWithPrime) {
    GaloisField::setPrime(7);

    EXPECT_NO_THROW(GaloisField a = 3);
    EXPECT_NO_THROW(GaloisField b = 0);
    EXPECT_NO_THROW(GaloisField c = 6);
    EXPECT_NO_THROW(GaloisField d = 7);
    EXPECT_NO_THROW(GaloisField e = 10);
    EXPECT_NO_THROW(GaloisField f = -1);
}

TEST_F(GaloisFieldTest, ValueNormalization) {
    GaloisField::setPrime(7);

    GaloisField a = 10;
    GaloisField b = 7;
    GaloisField c = -1;
    GaloisField d = -8;

    EXPECT_EQ(a.toInt(), 3);
    EXPECT_EQ(b.toInt(), 0);
    EXPECT_EQ(c.toInt(), 6);
    EXPECT_EQ(d.toInt(), 6);
}

TEST_F(GaloisFieldTest, Addition) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField b = 5;
    GaloisField c = a + b;

    EXPECT_EQ(c.toInt(), 1);

    GaloisField d = 6;
    GaloisField e = 2;
    GaloisField f = d + e;

    EXPECT_EQ(f.toInt(), 1);
}

TEST_F(GaloisFieldTest, Subtraction) {
    GaloisField::setPrime(7);

    GaloisField a = 5;
    GaloisField b = 3;
    GaloisField c = a - b;

    EXPECT_EQ(c.toInt(), 2);

    GaloisField d = 2;
    GaloisField e = 5;
    GaloisField f = d - e;

    EXPECT_EQ(f.toInt(), 4);
}

TEST_F(GaloisFieldTest, Multiplication) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField b = 4;
    GaloisField c = a * b;

    EXPECT_EQ(c.toInt(), 5);

    GaloisField d = 6;
    GaloisField e = 6;
    GaloisField f = d * e;

    EXPECT_EQ(f.toInt(), 1);
}

TEST_F(GaloisFieldTest, Division) {
    GaloisField::setPrime(7);

    GaloisField a = 6;
    GaloisField b = 2;
    GaloisField c = a / b;

    EXPECT_EQ(c.toInt(), 3);

    GaloisField zero = 0;
    EXPECT_THROW(a / zero, std::invalid_argument);
}

TEST_F(GaloisFieldTest, CompoundAssignment) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField b = 4;

    a += b;
    EXPECT_EQ(a.toInt(), 0);

    a = 5;
    a -= 2;
    EXPECT_EQ(a.toInt(), 3);

    a = 3;
    a *= 2;
    EXPECT_EQ(a.toInt(), 6);

    a = 6;
    a /= 2;
    EXPECT_EQ(a.toInt(), 3);
}

TEST_F(GaloisFieldTest, UnaryOperators) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField b = +a;
    GaloisField c = -a;

    EXPECT_EQ(b.toInt(), 3);
    EXPECT_EQ(c.toInt(), 4);

    GaloisField zero = 0;
    GaloisField negZero = -zero;
    EXPECT_EQ(negZero.toInt(), 0);
}

TEST_F(GaloisFieldTest, ComparisonOperators) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField b = 3;
    GaloisField c = 4;

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
}

TEST_F(GaloisFieldTest, AdditiveInverse) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField inv = a.additiveInverse();

    EXPECT_EQ(inv.toInt(), 4);
    EXPECT_EQ((a + inv).toInt(), 0);
}

TEST_F(GaloisFieldTest, MultiplicativeInverse) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField inv = a.multiplicativeInverse();

    EXPECT_EQ(inv.toInt(), 5);
    EXPECT_EQ((a * inv).toInt(), 1);

    GaloisField zero = 0;
    EXPECT_THROW(zero.multiplicativeInverse(), std::invalid_argument);
}

TEST_F(GaloisFieldTest, PowerOperation) {
    GaloisField::setPrime(7);

    GaloisField a = 3;

    EXPECT_EQ(a.power(0).toInt(), 1);
    EXPECT_EQ(a.power(1).toInt(), 3);
    EXPECT_EQ(a.power(2).toInt(), 2);
    EXPECT_EQ(a.power(3).toInt(), 6);
    EXPECT_EQ(a.power(-1).toInt(), 5);
    EXPECT_EQ(a.power(-2).toInt(), 4);
}


TEST_F(GaloisFieldTest, CopyAndAssignment) {
    GaloisField::setPrime(7);

    GaloisField a = 3;
    GaloisField b = a;
    GaloisField c = 5;
    c = a;

    EXPECT_EQ(a.toInt(), 3);
    EXPECT_EQ(b.toInt(), 3);
    EXPECT_EQ(c.toInt(), 3);

    a += 1;
    EXPECT_EQ(a.toInt(), 4);
    EXPECT_EQ(b.toInt(), 3);
    EXPECT_EQ(c.toInt(), 3);
}

TEST_F(GaloisFieldTest, DifferentPrimes) {
    GaloisField::setPrime(11);

    GaloisField a = 7;
    GaloisField b = 8;
    GaloisField c = a + b;

    EXPECT_EQ(c.toInt(), 4);

    GaloisField::setPrime(13);

    GaloisField d = 12;
    GaloisField e = 5;
    GaloisField f = d * e;

    EXPECT_EQ(f.toInt(), 8);
}

TEST_F(GaloisFieldTest, LargeOperations) {
    GaloisField::setPrime(97);

    GaloisField a = 50;
    GaloisField result = a;

    for (int i = 1; i < 50; ++i) {
        result = result * GaloisField(i) + GaloisField(i);
    }

    EXPECT_GE(result.toInt(), 0);
    EXPECT_LT(result.toInt(), 97);
}
