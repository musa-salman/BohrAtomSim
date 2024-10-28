#include "orbital_math.h"
#include "utils/constants.h"

long double compute_r_dot_dot(long double mass, long double r,
                              long double charge, long double k) {
    long double arg1;
    long double arg2;

    long double r_sqr = r * r;

    arg1 = k * k * H_BAR_SQR;
    arg1 /= mass * r_sqr * r;

    arg2 = charge * charge;
    arg2 /= r_sqr;

    arg1 = arg1 - arg2;

    arg1 /= mass;

    return arg1;
}