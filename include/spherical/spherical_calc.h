#ifndef SPHERE_CALC_H
#define SPHERE_CALC_H

/**
 * @brief Calculates the starting point for theta
 *
 *          THETA_MIN = arcsin( n_phi/K )
 *
 * @param n_phi
 * @param k
 * @return long double
 */
long double sphere_calc_theta_min(long double n_phi, long double k);

long double sphere_calc_theta_dot(long double k, long double mass, long double r);

/**
 * @brief Calculates the angular acceleration of the electron
 * where
 *
 *        THETA_DOT_DOT =  (sin(theta) * cos(theta) * phi_dot^2 )-( (r_dot / r) * 2* theta_dot )
 *
 * @param r electrons distance from the center of rotation
 * @param r_dot electrons speed
 * @param theta
 * @param theta_dot
 * @param phi_dot
 * @return long double
 */
long double sphere_calc_theta_dot_dot(long double r, long double r_dot, long double theta, long double theta_dot, long double phi_dot);

long double sphere_calc_phi_dot(long double n_phi, long double theta, long double mass, long double r);

#endif // SPHERE_CALC_H