#include <math.h>
#include <stdlib.h>

#include "orbital_math.h"

long double compute_r_dot_dot(long double r, long double k) {
    long double arg1;
    long double arg2;

    long double r_sqr = r * r;

    arg1 = k * k * H_BAR_SQR;
    arg1 /= ELECTRON_CHARGE * r_sqr * r;

    arg2 = ELECTRON_CHARGE * ELECTRON_CHARGE;
    arg2 /= r_sqr;

    arg1 = arg1 - arg2;

    arg1 /= ELECTRON_MASS;

    return arg1;
}

struct radial_bounds compute_radial_limits(long double principle,
                                           long double angular) {
    long double a = principle * principle;
    long double b = angular / principle;
    b *= a;

    long double dis = a * a;
    dis -= b * b;

    dis = sqrtl(dis);

    struct radial_bounds radial_bounds;
    radial_bounds.r_min = radial_bounds.r_max = a;
    radial_bounds.r_min -= dis;
    radial_bounds.r_max += dis;

    radial_bounds.r_min *= BOHR_R;
    radial_bounds.r_max *= BOHR_R;

    return radial_bounds;
}

long double compute_angular_rate(long double angular, long double radius) {
    long double arg1 = angular * H_BAR;
    arg1 /= ELECTRON_MASS * radius * radius;

    return arg1;
}

long double compute_rel_angular_rate(long double angular, long double radius,
                                     long double gamma) {
    long double arg1 = angular * H_BAR;
    arg1 /= radius * radius * ELECTRON_MASS * gamma;

    return arg1;
}

long double compute_gamma(long double angular, long double radius,
                          long double r_dot) {
    long double arg1 = C;
    arg1 *= ELECTRON_MASS * radius;
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

long double compute_rel_r_dot_dot(long double angular, long double gamma,
                                  long double r, long double r_dot) {
    long double arg1;
    long double arg2;
    long double arg3;

    long double r_sq = r * r;

    arg1 = angular * angular * H_BAR_SQR;
    arg1 /= gamma * ELECTRON_MASS * r_sq * r;

    arg2 = ELECTRON_CHARGE * ELECTRON_CHARGE;
    arg2 /= r_sq;

    arg3 = r_dot / C;
    arg3 *= arg3;
    arg3 = 1 - arg3;

    arg2 *= arg3;

    arg1 = arg1 - arg2;

    arg1 /= gamma * ELECTRON_MASS;

    return arg1;
}

long double compute_theta_min(long double n_phi, long double k) {
    return asinl(n_phi / k);
}

long double compute_spherical_phi_dot(long double n_phi, long double theta,
                                      long double radius) {
    long double arg1 = n_phi * H_BAR;

    long double arg2 = sinl(theta);
    arg2 *= arg2;
    arg2 *= ELECTRON_MASS * radius * radius;

    return arg1 / arg2;
}

struct vector3 *spherical_to_cartesian(long double radius, long double theta,
                                       long double phi) {
    struct vector3 *vec = malloc(sizeof(*vec));
    if (!vec) {
        return NULL;
    }

    vec->x = radius * sinl(theta) * cosl(phi);
    vec->y = radius * sinl(theta) * sinl(phi);
    vec->z = radius * cosl(theta);

    return vec;
}

#define VEC3_DOT(v1, v2) (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z)

long double compute_angular_distance(const struct vector3 *v1,
                                     const struct vector3 *v2) {
    const long double dot = VEC3_DOT(v1, v2);
    const long double mag1 = VEC3_DOT(v1, v1);
    const long double mag2 = VEC3_DOT(v2, v2);

    return acosl(dot / sqrtl(mag1 * mag2));
}

long double compute_sphere_theta_dot_dot(long double r, long double r_dot,
                                         long double theta,
                                         long double theta_dot,
                                         long double phi_dot) {
    long double arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    long double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    return arg1 - arg2;
}

long double compute_sphere_rel_theta_dot_dot(long double r, long double r_dot,
                                             long double theta,
                                             long double theta_dot,
                                             long double phi_dot,
                                             long double gamma) {
    long double arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    long double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    long double arg3 = gamma * ELECTRON_MASS * C * C * r;
    arg3 = ELECTRON_CHARGE * ELECTRON_CHARGE / arg3;
    arg3 = 1 - arg3;
    arg2 *= arg3;

    return arg1 - arg2;
}