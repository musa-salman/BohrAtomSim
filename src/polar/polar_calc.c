#include <math.h>
#include <stdlib.h>

#include "polar/polar_calc.h"
#include "utils/constants.h"

long double compute_phi_dot(long double l, long double mass, long double r) {
    return l / (mass * r * r);
}

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

struct radial_bounds *compute_radial_limits(long double principle,
                                            long double angular) {
    long double a = principle * principle;
    long double b = angular / principle;
    b *= a;

    long double dis = a * a;
    dis -= b * b;

    dis = sqrtl(dis);

    struct radial_bounds *radial_bounds = malloc(sizeof(*radial_bounds));
    radial_bounds->r_min = radial_bounds->r_max = a;
    radial_bounds->r_min -= dis;
    radial_bounds->r_max += dis;

    radial_bounds->r_min *= BOHR_R;
    radial_bounds->r_max *= BOHR_R;

    return radial_bounds;
}