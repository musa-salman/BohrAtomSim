#ifndef SPHERE_CALC_REL_H
#define SPHERE_CALC_REL_H

#include <stddef.h>

typedef struct spherical_calc_rel_params
{
    long double r;
    long double r_dot;
    long double theta;
    long double theta_dot;
    long double phi_dot;
    long double charge;
    long double mass;
    long double gamma;
} spherical_calc_rel_params;

long double rel_sphere_calc_theta_dot_dot(struct spherical_calc_rel_params params);

long double rel_sphere_calc_delta_phi(const long double *curr_max_vec, const long double *prev_max_vec);

long double *sph2cart(long double roh, long double phi, long double theta);

long double dot_product(const long double *v1, const long double *v2, size_t size);

#endif // SPHERE_CALC_REL_H