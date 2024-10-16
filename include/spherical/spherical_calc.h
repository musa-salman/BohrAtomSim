#ifndef SPHERE_CALC_H
#define SPHERE_CALC_H

/**
 * @brief Calculates the starting point for theta
 *
 *          THETA_MIN = arcsin( Nphi/K )
 *
 * @param Nphi
 * @param k
 * @return long double
 */
long double sphere_calc_theta_min(long double Nphi, long double k);

/**
    @brief Calculates R_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = (l^2)/(m*(r^3)) - (e^2)/(r^2)

    @param long double m = 9.109383e-28 long double e = 4.803 e-10 l_sqr = Calc_l_sqr r = r of the currnt itration
    @return long double r_dot_dot
 */
long double sphere_calc_r_dot_dot(long double m, long double r, long double e, long double k_sqr, long double Hbar_sqr);

/**
 * @brief Calcuates the angular acceleration of the electrion
 * where
 *
 *        THETA_DOT_DOT =  (sin(theta) * cos(theta) * phi_dot^2 )-( (r_dot / r) * 2* theta_dot )
 *
 * @param r electrons destance from the center of roation
 * @param r_dot electrons speed
 * @param theta
 * @param theta_dot
 * @param phi_dot
 * @return long double
 */
long double sphere_calc_theta_dot_dot(long double r, long double r_dot, long double theta, long double theta_dot, long double phi_dot);

long double sphere_calc_phi_dot(long double Nphi, long double Hbar, long double theta, long double mass, long double r);

long double sphere_calc_init_phi_dot(long double K, long double Hbar, long double nphi, long double mass, long double r);

long double sphere_calc_spc_case_r_dot_dot(long double r, long double theta_dot, long double charge, long double mass);

long double sphere_calc_spc_case_theta_dot(long double k, long double Hbar, long double mass, long double r);

#endif // SPHERE_CALC_H