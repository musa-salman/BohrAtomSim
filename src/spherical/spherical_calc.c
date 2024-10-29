#include <math.h>

#include "spherical/spherical_calc.h"

long double sphere_calc_theta_dot_dot(long double r, long double r_dot,
                                      long double theta, long double theta_dot,
                                      long double phi_dot) {

    long double arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    long double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    return arg1 - arg2;
}