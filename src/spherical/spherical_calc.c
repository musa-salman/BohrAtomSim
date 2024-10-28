#include <math.h>

#include "spherical/spherical_calc.h"
#include "utils/constants.h"

long double sphere_calc_theta_min(long double n_phi, long double k) {
    return asinl(n_phi / k);
}

long double sphere_calc_phi_dot(long double n_phi, long double theta,
                                long double mass, long double r) {
    // TODO: in docs gamma was part of the equation but in the code it is not

    long double arg1 = n_phi * H_BAR;

    long double arg2 = sinl(theta);
    arg2 *= arg2;
    arg2 *= mass * r * r;

    return arg1 / arg2;
}

long double sphere_calc_r_dot_dot(long double m, long double r, long double e,
                                  long double k_sqr) {

    long double arg1;
    long double arg2;

    long double r_sq = r * r;

    arg1 = k_sqr * H_BAR_SQR;
    arg1 /= m * r_sq * r;

    arg2 = e * e;
    arg2 /= r_sq;

    arg1 = arg1 - arg2;

    arg1 /= m;

    return arg1;
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

long double sphere_calc_spc_case_r_dot_dot(long double r, long double theta_dot,
                                           long double charge,
                                           long double mass) {
    long double arg1 = r * theta_dot * theta_dot;

    long double arg2 = charge * charge;

    arg2 /= mass * r * r;

    return arg1 - arg2;
}

long double sphere_calc_spc_case_theta_dot(long double k, long double mass,
                                           long double r) {

    long double arg1 = k * H_BAR;

    long double arg2 = r * r * mass;

    return arg1 / arg2;
}
