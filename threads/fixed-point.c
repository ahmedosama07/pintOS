#include "fixed-point.h"

real_t int_to_real(int32_t n) {
    real_t x;
    x.real = n;
    x.integer = x.real & PMASK;
    x.decimal = x.real & QMASK;
    return x;
}

int32_t to_int(real_t x) {
    int32_t n;
    if (x.decimal >= f/2) n = (x.integer + 1);
    else if (x.decimal <= f/2) n = (x.integer);
    return n;
}

real_t add_real(real_t x, real_t y) {
    return int_to_real(x.real + y.real);
}

real_t subtract_real(real_t x, real_t y) {
    return int_to_real(x.real - y.real);
}

real_t divide_real(real_t x, real_t y) {
    return int_to_real(((int64_t) x.real) * f / y.real);
}

real_t multiply_real(real_t x, real_t y) {
    return int_to_real(((int64_t) x.real) * y.real / f);
}

real_t add_int(real_t x, int32_t n) {
    return int_to_real(x.real + (f * n));
}

real_t subtract_int(real_t x, int32_t n) {
    return int_to_real(x.real - (f * n));
}

real_t divide_int(real_t x, int32_t n) {
    return int_to_real(x.real / n);
}

real_t multiply_int(real_t x, int32_t n) {
    return int_to_real(x.real * n);
}