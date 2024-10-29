#ifndef SPHERE_CALC_H
#define SPHERE_CALC_H

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

#endif // SPHERE_CALC_H