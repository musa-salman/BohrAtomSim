#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "orbital_math.h"
#include "utils/types.h"

scalar compute_r_dot_dot(scalar radius, quantum_angular angular) {
    const scalar result = ((SQUARE(angular) / radius) - 1) / SQUARE(radius);
    return result;
}

struct radial_bounds compute_radial_limits(quantum_principle principle,
                                           quantum_angular angular) {
    const scalar term1 = SQUARE(principle);
    const scalar term2 = SQUARE(angular) / term1;

    const scalar term3 = sqrt(1 - term2);

    struct radial_bounds radial_bounds;

    radial_bounds.r_min = term1 * (1 - term3);
    radial_bounds.r_max = term1 * (1 + term3);

    return radial_bounds;
}

scalar compute_angular_rate(quantum_angular angular, scalar radius) {
    return angular / SQUARE(radius);
}

scalar compute_gamma(quantum_angular angular, scalar radius, scalar r_dot,
                     scalar r_max) {
    const volatile scalar inv_r_prime = (r_max / radius);

    const scalar term1 = angular * INV_SQRT_SPEED_OF_LIGHT / (SQUARE(radius));
    const scalar term2 = SQUARE(r_dot) * INV_SQRT_SPEED_OF_LIGHT;

    const scalar result = sqrt((1 + term1) / (1 - term2));
    return result;
}

scalar compute_rel_r_dot_dot(quantum_angular angular, scalar gamma,
                             scalar radius, scalar r_dot, scalar r_max) {

    const volatile scalar r_inv = (r_max / radius) / r_max;
    const scalar term1 = r_inv * SQUARE(angular) / gamma;
    const scalar term2 = SQUARE(r_dot) * INV_SQRT_SPEED_OF_LIGHT;

    const scalar result = r_inv * r_inv * (term1 + term2 - 1) / gamma;

    return result;
}

scalar compute_theta_min(scalar n_phi, quantum_angular angular) {
    return asin(n_phi / angular);
}

scalar compute_spherical_phi_dot(scalar n_phi, scalar theta, scalar radius) {
    const scalar sin_theta = sin(theta);
    const scalar result = n_phi / (SQUARE(radius * sin_theta));

    return result;
}

scalar compute_sphere_rel_phi_dot(scalar n_phi, scalar theta, scalar radius,
                                  scalar gamma, scalar r_max) {

    const scalar constant = r_max * r_max;

    const scalar sin_theta = sin(theta);
    const volatile scalar normalized_result =
        constant / (radius * radius * sin_theta * sin_theta * gamma);

    const scalar result = normalized_result * n_phi / constant;

    return result;
}

struct vector3 *spherical_to_cartesian(scalar radius, scalar theta,
                                       scalar phi) {
    struct vector3 *vec = malloc(sizeof(*vec));
    if (!vec) {
        return NULL;
    }

    vec->x = radius * sin(theta) * cos(phi);
    vec->y = radius * sin(theta) * sin(phi);
    vec->z = radius * cos(theta);

    return vec;
}

#define VEC3_DOT(v1, v2)                                                       \
    ((v1)->x * (v2)->x + (v1)->y * (v2)->y + (v1)->z * (v2)->z)

scalar cross_magnitude(const struct vector3 *v1, const struct vector3 *v2) {
    return sqrt(SQUARE(v1->y * v2->z - v1->z * v2->y) +
                SQUARE(v1->z * v2->x - v1->x * v2->z) +
                SQUARE(v1->x * v2->y - v1->y * v2->x));
}

scalar compute_angular_distance(const struct vector3 *v1,
                                const struct vector3 *v2) {
    const scalar dot = VEC3_DOT(v1, v2);
    const scalar cross_mag = cross_magnitude(v1, v2);

    const scalar result = atan2(cross_mag, dot);

    return result;
}

scalar compute_sphere_theta_dot_dot(scalar r, scalar r_dot, scalar theta,
                                    scalar theta_dot, scalar phi_dot) {
    const scalar term1 = sin(theta) * cos(theta) * SQUARE(phi_dot);
    const scalar term2 = 2 * r_dot * theta_dot / r;

    const scalar result = term1 - term2;

    return result;
}

scalar compute_sphere_rel_theta_dot_dot(scalar r, scalar r_dot, scalar theta,
                                        scalar theta_dot, scalar phi_dot,
                                        scalar gamma, scalar r_max) {
    const scalar term1 = sin(theta) * cos(theta) * SQUARE(phi_dot);

    const volatile scalar r_inv = (r_max / r) / r_max;
    const scalar term2 = 2 * r_dot * theta_dot * r_inv;

    // Add regularization to prevent issues in term3
    const scalar term3 = r_inv * (1 / gamma) * INV_SQRT_SPEED_OF_LIGHT;

    const scalar result = term1 - term2 * (1 - term3);

    return result;
}