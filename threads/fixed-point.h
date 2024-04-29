#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>

// Number of bits representing the fractional part
#define Q 14

// Conversion factor from integer to fixed-point
#define F (1 << Q)

// Structure to represent fixed-point numbers
typedef struct
{
    int32_t value;       // Fixed-point representation
} real_t;

/**
 * Converts an integer to a fixed-point representation.
 * @param n The integer to be converted.
 * @return The fixed-point representation of the integer.
 */
static inline real_t to_real(int32_t n) {
    return (real_t){n * F};
}

/**
 * Converts a fixed-point number to an integer.
 * @param x The fixed-point number to be converted.
 * @return The integer representation of the fixed-point number.
 */
static inline int32_t to_int(real_t x) {
    return x.value / F;
}

/**
 * Rounds a fixed-point number to an integer.
 * @param x The fixed-point number to be converted.
 * @return The integer representation of the fixed-point number.
 */
static inline int32_t round_real(real_t x) {
    if (x.value >= 0)
        return (x.value + (F / 2)) /F;
    return (x.value - (F / 2)) /F;
}

/**
 * Adds two fixed-point numbers.
 * @param x The first fixed-point number.
 * @param y The second fixed-point number.
 * @return The result of the addition.
 */
static inline real_t add_real(real_t x, real_t y) {
    return (real_t){x.value + y.value};
}

/**
 * Subtracts one fixed-point number from another.
 * @param x The fixed-point number to subtract from.
 * @param y The fixed-point number to subtract.
 * @return The result of the subtraction.
 */
static inline real_t subtract_real(real_t x, real_t y) {
    return (real_t){x.value - y.value};
}

/**
 * Divides one fixed-point number by another.
 * @param x The numerator.
 * @param y The denominator.
 * @return The result of the division.
 */
static inline real_t divide_real(real_t x, real_t y) {
    return (real_t){((int64_t)x.value) * F / y.value};
}

/**
 * Multiplies two fixed-point numbers.
 * @param x The first fixed-point number.
 * @param y The second fixed-point number.
 * @return The result of the multiplication.
 */
static inline real_t multiply_real(real_t x, real_t y) {
    return (real_t){((int64_t)x.value) * y.value / F};
}

/**
 * Adds an integer to a fixed-point number.
 * @param x The fixed-point number.
 * @param n The integer to add.
 * @return The result of the addition.
 */
static inline real_t add_int(real_t x, int32_t n) {
    return (real_t){x.value + n * F};
}

/**
 * Subtracts an integer from a fixed-point number.
 * @param x The fixed-point number.
 * @param n The integer to subtract.
 * @return The result of the subtraction.
 */
static inline real_t subtract_int(real_t x, int32_t n) {
    return (real_t){x.value - n * F};
}

/**
 * Divides a fixed-point number by an integer.
 * @param x The fixed-point number.
 * @param n The integer denominator.
 * @return The result of the division.
 */
static inline real_t multiply_int(real_t x, int32_t n) {
    return (real_t){x.value * n};
}

/**
 * Multiplies a fixed-point number by an integer.
 * @param x The fixed-point number.
 * @param n The integer multiplier.
 * @return The result of the multiplication.
 */
static inline real_t divide_int(real_t x, int32_t n) {
    return (real_t){x.value / n};
}


#endif /* threads/fixed-point.h */
