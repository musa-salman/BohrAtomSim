#include <math.h>
#include <stdlib.h>

#include "orbital_math.h"
#include "utils/types.h"

scalar compute_r_dot_dot(scalar r, quantum_angular angular) {
    scalar arg1;
    scalar arg2;

    scalar r_sqr = r * r;

    arg1 = angular * angular * H_BAR_SQR;
    arg1 /= ELECTRON_CHARGE * r_sqr * r;

    arg2 = ELECTRON_CHARGE * ELECTRON_CHARGE;
    arg2 /= r_sqr;

    arg1 = arg1 - arg2;

    arg1 /= ELECTRON_MASS;

    return arg1;
}

struct radial_bounds compute_radial_limits(quantum_principle principle,
                                           quantum_angular angular) {
    scalar a = principle * principle;
    scalar b = (scalar)angular / principle;
    b *= a;

    scalar dis = a * a;
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

scalar compute_angular_rate(quantum_angular angular, scalar radius) {
    scalar arg1 = angular * H_BAR;
    arg1 /= ELECTRON_MASS * radius * radius;

    return arg1;
}

scalar compute_rel_angular_rate(quantum_angular angular, scalar radius,
                                scalar gamma) {
    scalar arg1 = angular * H_BAR;
    arg1 /= radius * radius * ELECTRON_MASS * gamma;

    return arg1;
}

scalar compute_gamma(quantum_angular angular, scalar radius, scalar r_dot) {
    scalar arg1 = C;
    arg1 *= ELECTRON_MASS * radius;
    arg1 = (angular * H_BAR) / arg1;
    arg1 *= arg1;
    arg1 += 1;

    scalar arg2 = C;
    arg2 = r_dot / arg2;
    arg2 *= arg2;
    arg2 = 1 - arg2;

    arg1 = sqrtl(arg1 / arg2);

    return arg1;
}

scalar compute_rel_r_dot_dot(quantum_angular angular, scalar gamma, scalar r,
                             scalar r_dot) {
    scalar arg1;
    scalar arg2;
    scalar arg3;

    scalar r_sq = r * r;

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

scalar compute_theta_min(scalar n_phi, quantum_angular angular) {
    return asinl(n_phi / angular);
}

scalar compute_spherical_phi_dot(scalar n_phi, scalar theta, scalar radius) {
    scalar arg1 = n_phi * H_BAR;

    scalar arg2 = sinl(theta);
    arg2 *= arg2;
    arg2 *= ELECTRON_MASS * radius * radius;

    return arg1 / arg2;
}

struct vector3 *spherical_to_cartesian(scalar radius, scalar theta,
                                       scalar phi) {
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

scalar compute_angular_distance(const struct vector3 *v1,
                                const struct vector3 *v2) {
    const scalar dot = VEC3_DOT(v1, v2);
    const scalar mag1 = VEC3_DOT(v1, v1);
    const scalar mag2 = VEC3_DOT(v2, v2);

    return acosl(dot / sqrtl(mag1 * mag2));
}

scalar compute_sphere_theta_dot_dot(scalar r, scalar r_dot, scalar theta,
                                    scalar theta_dot, scalar phi_dot) {
    scalar arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    scalar arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    return arg1 - arg2;
}

scalar compute_sphere_rel_theta_dot_dot(scalar r, scalar r_dot, scalar theta,
                                        scalar theta_dot, scalar phi_dot,
                                        scalar gamma) {
    scalar arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    scalar arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    scalar arg3 = gamma * ELECTRON_MASS * C * C * r;
    arg3 = ELECTRON_CHARGE * ELECTRON_CHARGE / arg3;
    arg3 = 1 - arg3;
    arg2 *= arg3;

    return arg1 - arg2;
}