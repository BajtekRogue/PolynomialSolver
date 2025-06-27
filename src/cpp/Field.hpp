#ifndef FIELD_HPP
#define FIELD_HPP

#include <iostream>
#include <string>

/**
 * Abstract base class representing an algebraic field.
 *
 * A field is an algebraic structure with binary operations addition and multiplication
 * that satisfies the field axioms:
 * 1. Closure under addition and multiplication
 * 2. Associativity of addition and multiplication
 * 3. Commutativity of addition and multiplication
 * 4. Existence of additive identity (zero)
 * 5. Existence of multiplicative identity (one)
 * 6. Existence of additive inverses
 * 7. Existence of multiplicative inverses (except for zero)
 * 8. Distributivity of multiplication over addition
 */
template<typename F> class Field {

public:
    virtual ~Field() = default;

    //  Arithmetic operations
    virtual F operator+(const F& other) const = 0;
    virtual F operator-(const F& other) const = 0;
    virtual F operator*(const F& other) const = 0;
    virtual F operator/(const F& other) const = 0;

    //  Compound assignment operators
    virtual F& operator+=(const F& other) = 0;
    virtual F& operator-=(const F& other) = 0;
    virtual F& operator*=(const F& other) = 0;
    virtual F& operator/=(const F& other) = 0;

    //  Unary operators
    virtual F operator+() const = 0;
    virtual F operator-() const = 0;

    //  Comparison operators
    virtual bool operator==(const F& other) const = 0;
    virtual bool operator!=(const F& other) const = 0;
    virtual bool operator<(const F& other) const = 0;
    virtual bool operator<=(const F& other) const = 0;
    virtual bool operator>(const F& other) const = 0;
    virtual bool operator>=(const F& other) const = 0;

    //  Assignment operator
    virtual F& operator=(const F& other) = 0;

    //  Utility functions
    virtual F additiveInverse() const = 0;
    virtual F multiplicativeInverse() const = 0;
    virtual std::string toString() const = 0;
    virtual F power(int64_t exp) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const Field& field) {
        return os << field.toString();
    }

    //  Identity elements
    const static F zero;
    const static F one;

protected:
    Field() = default;
    Field(const Field&) = default;
    Field& operator=(const Field&) = default;
};

#endif //  FIELD_HPP