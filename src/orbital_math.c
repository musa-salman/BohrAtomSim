#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "orbital_math.h"
#include "utils/types.h"

scalar compute_r_dot_dot(scalar radius, quantum_angular angular) {
    const scalar result = (SQUARE(angular) / radius - 1) / SQUARE(radius);
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

scalar compute_gamma(quantum_angular angular, scalar radius, scalar r_dot) {
    const scalar term1 =
        SQUARE(angular) / (SPEED_OF_LIGHT_SQUARE * SQUARE(radius));
    const scalar term2 = SQUARE(r_dot) / SPEED_OF_LIGHT_SQUARE;

    const scalar result = sqrt((1 + term1) / (1 - term2));
    return result;
}

scalar compute_rel_r_dot_dot(quantum_angular angular, scalar gamma,
                             scalar radius, scalar r_dot) {

    const scalar term1 = SQUARE(angular) / (gamma * radius);
    const scalar term2 = SQUARE(r_dot) / SPEED_OF_LIGHT_SQUARE;

    const scalar result = (term1 + term2 - 1) / (gamma * SQUARE(radius));

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

    return atan2f(cross_mag, dot);
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
                                        scalar gamma) {

    const scalar term1 = sin(theta) * cos(theta) * SQUARE(phi_dot);
    const scalar term2 = 2 * r_dot * theta_dot / r;
    const scalar term3 = 1 / (gamma * SPEED_OF_LIGHT_SQUARE * r);

    const scalar result = term1 - term2 * (1 - term3);

    return result;
}