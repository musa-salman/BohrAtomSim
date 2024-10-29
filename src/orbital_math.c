#include <math.h>
#include <stdlib.h>

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

long double compute_angular_rate(long double angular, long double mass,
                                 long double radius) {
    long double arg1 = angular * H_BAR;
    arg1 /= mass * radius * radius;

    return arg1;
}

long double compute_rel_angular_rate(long double angular, long double mass,
                                     long double radius, long double gamma) {
    long double arg1 = angular * H_BAR;
    arg1 /= radius * radius * mass * gamma;

    return arg1;
}

long double compute_rel_gamma(long double angular, long double mass,
                              long double radius, long double r_dot) {
    long double arg1 = C;
    arg1 *= mass * radius;
    arg1 = (angular * H_BAR) / arg1;
    arg1 *= arg1;
    arg1 += 1;

    long double arg2 = C;
    arg2 = r_dot / arg2;
    arg2 *= arg2;
    arg2 = 1 - arg2;

    arg1 = sqrtl(arg1 / arg2);

    return arg1;
}

long double compute_rel_r_dot_dot(long double angular, long double mass,
                                  long double gamma, long double r,
                                  long double charge, long double r_dot) {
    long double arg1;
    long double arg2;
    long double arg3;

    long double r_sq = r * r;

    arg1 = angular * angular * H_BAR_SQR;
    arg1 /= gamma * mass * r_sq * r;

    arg2 = charge * charge;
    arg2 /= r_sq;

    arg3 = r_dot / C;
    arg3 *= arg3;
    arg3 = 1 - arg3;

    arg2 *= arg3;

    arg1 = arg1 - arg2;

    arg1 /= gamma * mass;

    return arg1;
}

long double compute_theta_min(long double n_phi, long double k) {
    return asinl(n_phi / k);
}

long double compute_spherical_phi_dot(long double n_phi, long double theta,
                                      long double mass, long double r) {
    long double arg1 = n_phi * H_BAR;

    long double arg2 = sinl(theta);
    arg2 *= arg2;
    arg2 *= mass * r * r;

    return arg1 / arg2;
}