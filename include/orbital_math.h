#ifndef ORBITAL_MATH_H
#define ORBITAL_MATH_H

#include <math.h>

#include "utils/types.h"

/*
*************************************************************************************************************
*** All calculations are done according to Atomic Structure And Spectral Lines
*Vol. I by Arnold Sommerfeld ***
*************************************************************************************************************
*/

#define SPEED_OF_LIGHT_SQUARE FLOAT_LITERAL_SUFFIX(18778.86507043055614177) // in a.u. of velocity sqr

#define HALF_PI FLOAT_LITERAL_SUFFIX(1.570796326794896557998981)
#define PI FLOAT_LITERAL_SUFFIX(3.14159265358979323846)
#define TOW_PI FLOAT_LITERAL_SUFFIX(6.28318530717958647692)

struct radial_bounds {
    scalar r_min;
    scalar r_max;
};

struct vector3 {
    scalar x;
    scalar y;
    scalar z;
};

#define sin(x) _Generic((x), float: sinf, default: sin, long double: sinl)(x)
#define cos(x) _Generic((x), float: cosf, default: cos, long double: cosl)(x)
#define asin(x)                                                                \
    _Generic((x), float: asinf, default: asin, long double: asinl)(x)
#define acos(x)                                                                \
    _Generic((x), float: acosf, default: acos, long double: acosl)(x)
#define sqrt(x)                                                                \
    _Generic((x), float: sqrtf, default: sqrt, long double: sqrtl)(x)

/**
    Calculates r_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = ((k*h_bar)^2)/(m*(r^3)) - (e^2)/(r^2)
        l = k * H_BAR
        r_dot_dot = (k^2 * H_BAR_SQR) / (m^2*r^3) - e^2 / (m * r^2)
*/
scalar compute_r_dot_dot(scalar radius, quantum_angular angular);

/**
    Calculates the R_max and the R_min for a given electron orbit
    where

        (1) r_min + r_max = 2*a
        (2) r_min*max = b^2

        where a = n^2 * BOHR_R
        and   b = k/n * a
*/
struct radial_bounds compute_radial_limits(quantum_principle principal,
                                           quantum_angular angular);

#define POLAR_PHI_DOT(angular, radius) compute_angular_rate(angular, radius)
#define SPHERICAL_THETA_DOT(angular, radius)                                   \
    compute_angular_rate(angular, radius)

/**
    Calculates the angular change rate of the electron movement
    where
        angular_velocity = (k * h_bar) / (mass * r^2)
*/
scalar compute_angular_rate(quantum_angular angular, scalar radius);

/**
 * @brief Calculates gamma  "change of the mass" of and electron
 *
 * @param angular
 * @param mass
 * @param radius
 * @param r_dot
 * @return double
 */
scalar compute_gamma(quantum_angular angular, scalar radius, scalar r_dot);

/**
    Calculates the angular change rate in relevistic
    where
        angular_velocity = (k * H_BAR) / (r^2 * mass * gamma)
*/
#define REL_ANGULAR_RATE(angular, radius, gamma)                               \
    compute_angular_rate(angular, radius) / gamma

#define POLAR_PHI_DOT_REL(angular, radius, gamma)                              \
    REL_ANGULAR_RATE(angular, radius, gamma)
#define SPHERICAL_THETA_DOT_REL(angular, radius, gamma)                        \
    REL_ANGULAR_RATE(angular, radius, gamma)

/**
 * @brief Calculates R_dot_dot  with relativity incorporated "acceleration" of
 * and electron
 *
 *                      gamma*m*r_dot_dot = (l^2)/(gamma*m*(r^3)) -
 * (e^2)/(r^2)(1-(r_dot/c)^2)
 *
 * @param angular
 * @param gamma = calc_rel_gamma
 * @param r
 * @param r_dot
 * @return double
 */
scalar compute_rel_r_dot_dot(quantum_angular angular, scalar gamma,
                             scalar radius, scalar r_dot);

/**
 * @brief Calculates the starting point for theta
 *
 *          THETA_MIN = arcsin( n_phi/K )
 *
 * @param n_phi
 * @param k
 * @return scalar
 */
scalar compute_theta_min(scalar n_phi, quantum_angular angular);

#define REL_SPHERICAL_PHI_DOT(n_phi, theta, r, gamma)                          \
    compute_spherical_phi_dot(n_phi, theta, r) / gamma

scalar compute_spherical_phi_dot(scalar n_phi, scalar theta, scalar radius);

struct vector3 *spherical_to_cartesian(scalar radius, scalar theta, scalar phi);

scalar compute_angular_distance(const struct vector3 *v1,
                                const struct vector3 *v2);
/**
 * @brief Calculates the angular acceleration of the electron
 * where
 *
 *        THETA_DOT_DOT =  (sin(theta) * cos(theta) * phi_dot^2 )-( (r_dot / r)
 * * 2* theta_dot )
 *
 * @param r electrons distance from the center of rotation
 * @param r_dot electrons speed
 * @param theta
 * @param theta_dot
 * @param phi_dot
 * @return scalar
 */
scalar compute_sphere_theta_dot_dot(scalar radius, scalar r_dot, scalar theta,
                                    scalar theta_dot, scalar phi_dot);

scalar compute_sphere_rel_theta_dot_dot(scalar radius, scalar r_dot,
                                        scalar theta, scalar theta_dot,
                                        scalar phi_dot, scalar gamma);

#endif // ORBITAL_MATH_H