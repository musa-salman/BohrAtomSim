#include <math.h>
#include <stdlib.h>

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

long double rel_sphere_calc_delta_phi(const long double *curr_max_vec,
                                      const long double *prev_max_vec) {

    long double arg1 = dot_product(curr_max_vec, prev_max_vec, 3);

    long double rmax_curr = sqrtl(dot_product(curr_max_vec, curr_max_vec, 3));
    long double rmax_prev = sqrtl(dot_product(prev_max_vec, prev_max_vec, 3));
    arg1 /= (rmax_curr * rmax_prev);

    return acosl(arg1);
}

long double *sph2cart(long double roh, long double phi, long double theta) {

    long double *cords = malloc(sizeof(*cords) * 3);

    long double x = roh * cosl(phi) * sinl(theta);
    long double y = roh * sinl(phi) * sinl(theta);
    long double z = roh * cosl(theta);

    cords[0] = x;
    cords[1] = y;
    cords[2] = z;

    return cords;
}

long double dot_product(const long double *v1, const long double *v2,
                        size_t size) {
    long double sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += v1[i] * v2[i];
    }
    return sum;
}