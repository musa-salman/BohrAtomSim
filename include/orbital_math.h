#ifndef ORBITAL_MATH_H
#define ORBITAL_MATH_H

#include <math.h>
#include <sleef.h>

#include "utils/types.h"

/*
*************************************************************************************************************
*** All calculations are done according to Atomic Structure And Spectral Lines
*Vol. I by Arnold Sommerfeld ***
*************************************************************************************************************
*/

#define SPEED_OF_LIGHT_SQUARE FLOAT_LITERAL_SUFFIX(18778.86507043055614177)
#define INV_SPEED_OF_LIGHT_SQUARED                                             \
    FLOAT_LITERAL_SUFFIX(0.00005325135444817764497014)

#define G_FACTOR FLOAT_LITERAL_SUFFIX(-2.002319304)
#define S_Z_FACTOR FLOAT_LITERAL_SUFFIX(0.5)

#define HALF_PI FLOAT_LITERAL_SUFFIX(1.570796326794896557998981)
#define SIM_PI FLOAT_LITERAL_SUFFIX(3.14159265358979323846)
#define TWO_PI FLOAT_LITERAL_SUFFIX(6.28318530717958647692)

struct radial_bounds {
    scalar r_min;
    scalar r_max;
};

struct vector3 {
    scalar x;
    scalar y;
    scalar z;
};

#define sim_sin(x)                                                             \
    _Generic((x),                                                              \
        float: Sleef_sinf1_u35purecfma,                                        \
        double: Sleef_finz_sind1_u35purecfma,                                  \
        long double: sinl)(x)

#define sim_cos(x)                                                             \
    _Generic((x),                                                              \
        float: Sleef_cosf1_u35purecfma,                                        \
        double: Sleef_cosd1_u35purecfma,                                       \
        long double: cosl)(x)

#define sim_sqrt(x)                                                            \
    _Generic((x),                                                              \
        float: Sleef_sqrtf1_u35purecfma,                                       \
        double: Sleef_finz_sqrtd1_u35purecfma,                                 \
        long double: sqrtl)(x)

#define SQUARE(x) ((x) * (x))

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

scalar compute_phi_dot_0(quantum_angular angular, quantum_magnetic magnetic,
                         scalar radius);
/**
 * @brief Calculates gamma  "change of the mass" of and electron
 *
 * @param angular
 * @param mass
 * @param radius
 * @param r_dot
 * @return double
 */
static inline scalar compute_gamma(quantum_angular angular, scalar radius,
                                   scalar r_dot) {
    const scalar term1 =
        SQUARE(angular) / (SPEED_OF_LIGHT_SQUARE * SQUARE(radius));
    const scalar term2 = SQUARE(r_dot) / SPEED_OF_LIGHT_SQUARE;

    const scalar result = sim_sqrt((1 + term1) / (1 - term2));
    return result;
}

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
 * @brief Calculates R_dot_dot  with relativity incorporated
 * "acceleration" of and electron
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
static inline scalar compute_rel_r_dot_dot(quantum_angular angular,
                                           scalar gamma, scalar radius,
                                           scalar r_dot) {
    const scalar term1 = SQUARE(angular) / (gamma * radius);
    const scalar term2 = SQUARE(r_dot) / SPEED_OF_LIGHT_SQUARE;

    // Precompute reused terms
    const scalar radius_squared = radius * radius;
    const scalar reciprocal = 1.0 / (gamma * radius_squared);

    // Perform computation
    const scalar intermediate = term1 + term2 - 1.0;
    const scalar result = intermediate * reciprocal;

    return result;
}

/**
 * @brief Calculates the starting point for theta
 *
 *          THETA_MIN = arcsin(m/k)
 *
 * @param magnetic
 * @param angular
 * @return scalar
 */
scalar compute_theta_min(quantum_magnetic magnetic, quantum_angular angular);

#define REL_SPHERICAL_PHI_DOT(n_phi, theta, r, gamma)                          \
    compute_spherical_phi_dot(n_phi, theta, r) / gamma

scalar compute_sphere_rel_phi_dot_0(quantum_angular angular,
                                    quantum_magnetic magnetic, scalar radius,
                                    scalar gamma);

static inline scalar compute_sphere_rel_phi_dot(quantum_magnetic magnetic,
                                                scalar theta, scalar radius,
                                                scalar gamma) {
    const scalar sin_theta = sim_sin(theta);
    const scalar result = magnetic / (SQUARE(radius * sin_theta));

    return result / gamma;
}

scalar compute_spherical_phi_dot(quantum_magnetic magnetic, scalar theta,
                                 scalar radius);

struct vector3 *spherical_to_cartesian(scalar radius, scalar theta, scalar phi);

scalar compute_angular_distance(const struct vector3 *v1,
                                const struct vector3 *v2);
/**
 * @brief Calculates the angular acceleration of the electron
 * where
 *
 *        THETA_DOT_DOT =  (sin(theta) * cos(theta) * phi_dot^2 )-(
 * (r_dot / r)
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

static inline scalar compute_sphere_rel_theta_dot_dot(scalar r, scalar r_dot,
                                                      scalar theta,
                                                      scalar theta_dot,
                                                      scalar phi_dot,
                                                      scalar gamma) {

    const scalar term1 = 0.5 * sim_sin(2 * theta) * SQUARE(phi_dot);
    const scalar term2 = 2 * r_dot * theta_dot / r;
    const scalar term3 = 0.5 / (gamma * SPEED_OF_LIGHT_SQUARE * r);

    const scalar result = term1 - term2 * (1 - term3);

    return result;
}

scalar compute_sphere_phi_dot_dot(scalar radius, scalar r_dot, scalar theta,
                                  scalar theta_dot, scalar phi_dot);

static inline scalar compute_spin_gamma(scalar radius, scalar r_dot,
                                        scalar theta, scalar theta_dot,
                                        quantum_angular angular) {
    const scalar sin_theta = sim_sin(theta);

    const scalar term1 = 4 * SQUARE(SPEED_OF_LIGHT_SQUARE * radius);
    const scalar term2 =
        SPEED_OF_LIGHT_SQUARE * SQUARE(sin_theta * radius) + SQUARE(angular);
    const scalar term3 = term1 * term2;

    const scalar term4 = 4 * SPEED_OF_LIGHT_SQUARE * G_FACTOR * S_Z_FACTOR *
                         radius * SQUARE(sin_theta) * angular;
    const scalar term5 =
        SQUARE(G_FACTOR * S_Z_FACTOR) * SQUARE(SQUARE(sin_theta));
    const scalar term6 = -term3 + term4 - term5;

    const scalar term7 =
        SQUARE(theta_dot * radius) + SQUARE(r_dot) - SPEED_OF_LIGHT_SQUARE;
    const scalar term8 = term6 / term7;

    const scalar term9 = 2 * SPEED_OF_LIGHT_SQUARE * SQUARE(radius) * sin_theta;

    const scalar result = sim_sqrt(term8) / term9;

    return result;
}

static inline scalar compute_spin_r_ddot(scalar phi_dot, scalar radius,
                                         scalar r_dot, scalar theta,
                                         scalar theta_dot, scalar gamma) {
    const scalar sin_theta = sim_sin(theta);

    const scalar term1 = SQUARE(phi_dot * sin_theta) * radius;

    const scalar term2 = SQUARE(theta_dot) * radius;

    const scalar term3 = 1 / (gamma * SQUARE(radius));
    const scalar term4 = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;

    const scalar term5 = 0.5 * G_FACTOR * S_Z_FACTOR *
                         INV_SPEED_OF_LIGHT_SQUARED * phi_dot *
                         SQUARE(sin_theta) / (gamma * SQUARE(radius));
    const scalar term6 = 2 * theta_dot * r_dot * radius *
                         INV_SPEED_OF_LIGHT_SQUARED * sim_cos(theta) /
                         sin_theta;

    const scalar result =
        term1 + term2 - term3 * (1 - term4) - term5 * (1 - term6);

    return result;
}

static inline scalar compute_spin_theta_ddot(scalar phi_dot, scalar radius,
                                             scalar r_dot, scalar theta,
                                             scalar theta_dot, scalar gamma) {
    const scalar sin_cos_theta = sim_sin(theta) * sim_cos(theta);

    const scalar term1 = SQUARE(phi_dot) * sin_cos_theta;

    const scalar term2 = 2 * theta_dot * r_dot / radius;
    const scalar term3 = 0.5 * INV_SPEED_OF_LIGHT_SQUARED / (gamma * radius);

    const scalar term4 = G_FACTOR * S_Z_FACTOR *
                         SQUARE(INV_SPEED_OF_LIGHT_SQUARED) * phi_dot *
                         SQUARE(theta_dot) * sin_cos_theta / (gamma * radius);

    const scalar result = term1 - term2 * (1 - term3) + term4;

    return result;
}

static inline scalar compute_spin_phi_dot_0(quantum_angular angular,
                                            quantum_magnetic magnetic,
                                            scalar radius, scalar gamma) {
    const scalar term1 = SQUARE(angular) / (gamma * magnetic * SQUARE(radius));

    const scalar term2 = 0.5 * G_FACTOR * S_Z_FACTOR *
                         INV_SPEED_OF_LIGHT_SQUARED /
                         (gamma * SQUARE(radius) * radius);

    const scalar result = term1 - term2;

    return result;
}

static inline scalar compute_spin_phi_dot(scalar radius, scalar theta,
                                          scalar gamma,
                                          quantum_magnetic magnetic) {
    const scalar sin_theta = sim_sin(theta);

    const scalar term1 =
        magnetic / (gamma * SQUARE(radius) * SQUARE(sin_theta));

    const scalar term2 = 0.5 * G_FACTOR * S_Z_FACTOR *
                         INV_SPEED_OF_LIGHT_SQUARED /
                         (gamma * SQUARE(radius) * radius);

    const scalar result = term1 - term2;

    return result;
}

#endif // ORBITAL_MATH_H
