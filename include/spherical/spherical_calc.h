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

/**
    @brief Calculates R_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = (l^2)/(m*(r^3)) - (e^2)/(r^2)

    @param long double m = 9.109383e-28 long double e = 4.803 e-10 l_sqr = Calc_l_sqr r = r of the current iteration
    @return long double r_dot_dot
 */
long double sphere_calc_r_dot_dot(long double m, long double r, long double e, long double k_sqr, long double Hbar_sqr);

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

long double sphere_calc_phi_dot(long double n_phi, long double Hbar, long double theta, long double mass, long double r);

long double sphere_calc_init_phi_dot(long double K, long double Hbar, long double n_phi, long double mass, long double r);

long double sphere_calc_spc_case_r_dot_dot(long double r, long double theta_dot, long double charge, long double mass);

long double sphere_calc_spc_case_theta_dot(long double k, long double Hbar, long double mass, long double r);

#endif // SPHERE_CALC_H