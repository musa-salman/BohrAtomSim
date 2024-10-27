#include <math.h>

#include "polar/polar_calc_rel.h"
#include "utils/constants.h"

long double calc_rel_phi_dot(long double l, long double gamma, long double r,
                             long double m) {
    r *= r;

    long double arg1 = gamma * m;
    arg1 *= r;
    return l / arg1;
}

long double calc_rel_gamma(long double l, long double m, long double r,
                           long double r_dot) {
    long double arg1 = C;
    arg1 *= m * r;
    arg1 = l / arg1;
    arg1 *= arg1;
    arg1++;

    long double arg2 = C;
    arg2 = r_dot / arg2;
    arg2 *= arg2;
    arg2 = 1 - arg2;

    arg1 = sqrtl(arg1 / arg2);

    return arg1;
}

long double calc_rel_r_dot_dot(long double l_sqr, long double m,
                               long double gamma, long double r, long double e,
                               long double r_dot) {
    long double arg1;
    long double arg2;
    long double arg3;

    long double r_sq = r * r;

    arg1 = l_sqr;
    arg1 /= gamma * m * r_sq * r;

    arg2 = e * e;
    arg2 /= r_sq;

    arg3 = r_dot / C;
    arg3 *= arg3;
    arg3 = 1 - arg3;

    arg2 *= arg3;

    arg1 = arg1 - arg2;

    arg1 /= gamma * m;

    return arg1;
}