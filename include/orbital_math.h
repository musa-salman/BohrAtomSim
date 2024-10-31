#ifndef ORBITAL_MATH_H
#define ORBITAL_MATH_H

/*
*************************************************************************************************************
*** All calculations are done according to Atomic Structure And Spectral Lines
*Vol. I by Arnold Sommerfeld ***
*************************************************************************************************************
*/

#define BOHR_R 5.29177210903e-9        // in cm
#define H_BAR 1.0545718e-27            // in ergs
#define H_BAR_SQR 1.11212168135524e-54 // in ergs^2

#define ELECTRON_CHARGE 4.803204672997660e-10 // in esu
#define ELECTRON_MASS 9.10938356e-28          // in grams

#define PI 3.14159265358979323846
#define _2_PI 6.28318530717958647692
#define C 29979245800

struct radial_bounds {
    long double r_min;
    long double r_max;
};

struct vector3 {
    long double x;
    long double y;
    long double z;
};

/**
    Calculates r_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = ((l^2)/(m*(r^3)) - (e^2)/(r^2)
        l = k * H_BAR
        r_dot_dot = (k^2 * H_BAR_SQR / (m*r^3) - e^2 / (r^2)) / m
*/
long double compute_r_dot_dot(long double radius, long double angular);

/**
    Calculates the R_max and the R_min for a given electron orbit
    where

        (1) r_min + r_max = 2*a
        (2) r_min*max = b^2

        where a = n^2 * BOHR_R
        and   b = k/n * a
*/
struct radial_bounds compute_radial_limits(long double principal,
                                           long double angular);

#define POLAR_PHI_DOT(angular, radius) compute_angular_rate(angular, radius)
#define SPHERICAL_THETA_DOT(angular, radius)                                   \
    compute_angular_rate(angular, radius)

/**
    Calculates the angular change rate of the electron movement
    where
        angular_velocity = (k * h_bar) / (mass * r^2)
*/
long double compute_angular_rate(long double angular, long double radius);

/**
 * @brief Calculates gamma  "change of the mass" of and electron
 *
 * @param angular
 * @param mass
 * @param radius
 * @param r_dot
 * @return double
 */
long double compute_gamma(long double angular, long double radius,
                          long double r_dot);

#define POLAR_PHI_DOT_REL(angular, radius, gamma)                              \
    compute_rel_angular_rate(angular, radius, gamma)
#define SPHERICAL_THETA_DOT_REL(angular, radius, gamma)                        \
    compute_rel_angular_rate(angular, radius, gamma)

/**
    Calculates the angular change rate in relevistic
    where
        angular_velocity = (k * H_BAR) / (r^2 * mass * gamma)
*/
long double compute_rel_angular_rate(long double angular, long double radius,
                                     long double gamma);

/**
 * @brief Calculates R_dot_dot  with relativity incorporated "acceleration" of
 * and electron
 *
 *                      gamma*m*r_dot_dot = (l^2)/(gamma*m*(r^3)) - (e^2)/(r^2)
 *
 * @param angular
 * @param gamma = calc_rel_gamma
 * @param r
 * @param r_dot
 * @return double
 */
long double compute_rel_r_dot_dot(long double angular, long double gamma,
                                  long double radius, long double r_dot);

/**
 * @brief Calculates the starting point for theta
 *
 *          THETA_MIN = arcsin( n_phi/K )
 *
 * @param n_phi
 * @param k
 * @return long double
 */
long double compute_theta_min(long double n_phi, long double angular);

#define REL_SPHERICAL_PHI_DOT(n_phi, theta, r, gamma)                          \
    compute_spherical_phi_dot(n_phi, theta, r) / gamma

long double compute_spherical_phi_dot(long double n_phi, long double theta,
                                      long double radius);

struct vector3 *spherical_to_cartesian(long double radius, long double theta,
                                       long double phi);

long double compute_angular_distance(const struct vector3 *v1,
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
 * @return long double
 */
long double compute_sphere_theta_dot_dot(long double radius, long double r_dot,
                                         long double theta,
                                         long double theta_dot,
                                         long double phi_dot);

long double
compute_sphere_rel_theta_dot_dot(long double radius, long double r_dot,
                                 long double theta, long double theta_dot,
                                 long double phi_dot, long double gamma);

#endif // ORBITAL_MATH_H