#ifndef ORBITAL_MATH_H
#define ORBITAL_MATH_H

/*
*************************************************************************************************************
*** All calculations are done according to Atomic Structure And Spectral Lines Vol. I by Arnold Sommerfeld ***
*************************************************************************************************************
*/

struct radial_bounds
{
    long double r_min;
    long double r_max;
};

/**
    Calculates r_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = ((l^2)/(m*(r^3)) - (e^2)/(r^2)
        l = k * H_BAR
        r_dot_dot = (k^2 * H_BAR_SQR / (m*r^3) - e^2 / (r^2)) / m
*/
long double compute_r_dot_dot(long double mass, long double r, long double charge, long double k);

/**
    Calculates the R_max and the R_min for a given electron orbit
    where

        (1) r_min + r_max = 2*a
        (2) r_min*max = b^2

        where a = n^2 * BOHR_R
        and   b = k/n * a
*/
struct radial_bounds *compute_radial_limits(long double energy_level, long double k);

#define POLAR_PHI_DOT(angular, mass, radius) compute_angular_rate(angular, mass, radius)
#define SPHERICAL_THETA_DOT(angular, mass, radius) compute_angular_rate(angular, mass, radius)

/**
    Calculates the angular change rate of the electron movement
    where 
        angular_velocity = (k * h_bar) / (mass * r^2)
*/
long double compute_angular_rate(long double angular, long double mass, long double radius);

#define POLAR_PHI_DOT_REL(angular, mass, radius, gamma) compute_rel_angular_rate(angular, mass, radius, gamma)
#define SPHERICAL_THETA_DOT_REL(angular, mass, radius, gamma) compute_rel_angular_rate(angular, mass, radius, gamma)

/**
    Calculates the angular change rate in relevistic
    where
        angular_velocity = (k * H_BAR) / (r^2 * mass * gamma)
*/
long double compute_rel_angular_rate(long double angular, long double mass, long double radius, long double gamma);

/**
 * @brief Calculates gamma  "change of the mass" of and electron
 *
 * @param angular
 * @param mass
 * @param radius
 * @param r_dot
 * @return double
 */
long double compute_rel_gamma(long double angular, long double mass, long double radius, long double r_dot);

/**
 * @brief Calculates R_dot_dot  with relativity incorporated "acceleration" of and electron
 *
 *                      gamma*m*r_dot_dot = (l^2)/(gamma*m*(r^3)) - (e^2)/(r^2)
 *
 * @param l_sqr = (k * Hbar)^2
 * @param mass
 * @param gamma = calc_rel_gamma
 * @param r
 * @param charge
 * @param r_dot
 * @return double
 */
long double compute_rel_r_dot_dot(long double angular, long double mass,
                                  long double gamma, long double r,
                                  long double charge, long double r_dot);

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


#define REL_SPHERICAL_PHI_DOT(n_phi, theta, mass, r, gamma)                              \
    compute_spherical_phi_dot(n_phi, theta, mass, r) / gamma

long double compute_spherical_phi_dot(long double n_phi, long double theta, long double mass, long double r);

#endif // ORBITAL_MATH_H