#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>

// Number of bits representing the fractional part
#define q 14

// Conversion factor from integer to fixed-point
#define f (1<<q)

// Masks for separating integer and fractional parts
#define PMASK (-1)<<q
#define QMASK ~(PMASK)

// Macro to convert integer to fixed-point
#define to_real(n) int_to_real(f*n);

// Structure to represent fixed-point numbers
typedef struct
{
    int32_t real;       // Fixed-point representation
    int32_t integer;    // Integer part
    int32_t decimal;    // Fractional part
} real_t;

/**
 * Converts an integer to a fixed-point representation.
 * @param n The integer to be converted.
 * @return The fixed-point representation of the integer.
 */
real_t int_to_real(int32_t n);

/**
 * Converts a fixed-point number to an integer.
 * @param x The fixed-point number to be converted.
 * @return The integer representation of the fixed-point number.
 */
int32_t to_int(real_t x);

/**
 * Adds two fixed-point numbers.
 * @param x The first fixed-point number.
 * @param y The second fixed-point number.
 * @return The result of the addition.
 */
real_t add_real(real_t x, real_t y);

/**
 * Subtracts one fixed-point number from another.
 * @param x The fixed-point number to subtract from.
 * @param y The fixed-point number to subtract.
 * @return The result of the subtraction.
 */
real_t subtract_real(real_t x, real_t y);

/**
 * Divides one fixed-point number by another.
 * @param x The numerator.
 * @param y The denominator.
 * @return The result of the division.
 */
real_t multiply_real(real_t x, real_t y);

/**
 * Multiplies two fixed-point numbers.
 * @param x The first fixed-point number.
 * @param y The second fixed-point number.
 * @return The result of the multiplication.
 */
real_t divide_real(real_t x, real_t y);

/**
 * Adds an integer to a fixed-point number.
 * @param x The fixed-point number.
 * @param n The integer to add.
 * @return The result of the addition.
 */
real_t add_int(real_t x, int32_t n);

/**
 * Subtracts an integer from a fixed-point number.
 * @param x The fixed-point number.
 * @param n The integer to subtract.
 * @return The result of the subtraction.
 */
real_t subtract_int(real_t x, int32_t n);

/**
 * Divides a fixed-point number by an integer.
 * @param x The fixed-point number.
 * @param n The integer denominator.
 * @return The result of the division.
 */
real_t multiply_int(real_t x, int32_t n);

/**
 * Multiplies a fixed-point number by an integer.
 * @param x The fixed-point number.
 * @param n The integer multiplier.
 * @return The result of the multiplication.
 */
real_t divide_int(real_t x, int32_t n);


#endif /* threads/fixed-point.h */
