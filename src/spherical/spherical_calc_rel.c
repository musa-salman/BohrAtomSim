#include <math.h>

#include "spherical/spherical_calc_rel.h"
#include "utils/constants.h"

long double
rel_sphere_calc_theta_dot_dot(struct spherical_calc_rel_params params) {
    long double r = params.r;
    long double r_dot = params.r_dot;
    long double theta = params.theta;
    long double theta_dot = params.theta_dot;
    long double phi_dot = params.phi_dot;
    long double charge = params.charge;
    long double mass = params.mass;
    long double gamma = params.gamma;

    long double arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    long double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    long double arg3 = gamma * mass * C * C * r;
    arg3 = (charge * charge) / arg3;
    arg3 = 1 - arg3;
    arg2 *= arg3;

    return arg1 - arg2;
}