#include <math.h>

#include "spherical/spherical_calc.h"
#include "utils/constants.h"

long double sphere_calc_theta_min(long double n_phi, long double k) {
    return asinl(n_phi / k);
}

long double sphere_calc_phi_dot(long double n_phi, long double theta,
                                long double mass, long double r) {
    long double arg1 = n_phi * H_BAR;

    long double arg2 = sinl(theta);
    arg2 *= arg2;
    arg2 *= mass * r * r;

    return arg1 / arg2;
}

long double sphere_calc_theta_dot(long double k, long double mass,
                                  long double r) {

    long double arg1 = k * H_BAR;

    long double arg2 = r * r * mass;

    return arg1 / arg2;
}

long double sphere_calc_theta_dot_dot(long double r, long double r_dot,
                                      long double theta, long double theta_dot,
                                      long double phi_dot) {

    long double arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    long double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    return arg1 - arg2;
}