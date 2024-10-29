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

#endif // SPHERE_CALC_REL_H