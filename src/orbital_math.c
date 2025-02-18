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

scalar compute_sphere_phi_dot_dot(scalar radius, scalar r_dot, scalar theta,
                                  scalar theta_dot, scalar phi_dot) {
    const scalar term1 = r_dot * phi_dot / radius;
    const scalar term2 = cos(theta) * theta_dot * phi_dot / sin(theta);

    const scalar result = -2 * (term1 + term2);

    return result;
}

scalar compute_theta_min(quantum_magnetic magnetic, quantum_angular angular) {
    return asin((scalar)magnetic / angular);
}

scalar compute_phi_dot_0(quantum_angular angular, quantum_magnetic magnetic,
                         scalar radius) {
    return SQUARE(angular) / (SQUARE(radius) * magnetic);
}

scalar compute_spherical_phi_dot(quantum_magnetic magnetic, scalar theta,
                                 scalar radius) {
    const scalar sin_theta = sin(theta);
    const scalar result = (scalar)magnetic / (SQUARE(radius * sin_theta));

    return result;
}

scalar compute_sphere_rel_phi_dot_0(quantum_angular angular,
                                    quantum_magnetic magnetic, scalar radius,
                                    scalar gamma) {
    const scalar result = SQUARE(angular) / (SQUARE(radius) * magnetic);

    return result / gamma;
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