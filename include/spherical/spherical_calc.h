#ifndef SPHERE_CALC_H
#define SPHERE_CALC_H

/**
 * @brief Calculates the starting point for theta
 *
 *          THETA_MIN = arcsin( Nphi/K )
 *
 * @param Nphi
 * @param k
 * @return double
 */
double sphere_calc_theta_min(double Nphi, double k);

/**
    @brief Calculates R_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = (l^2)/(m*(r^3)) - (e^2)/(r^2)

    @param double m = 9.109383e-28 double e = 4.803 e-10 l_sqr = Calc_l_sqr r = r of the currnt itration
    @return double r_dot_dot
 */
double sphere_calc_r_dot_dot(double m, double r, double e, double k_sqr, double Hbar_sqr);

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
 * @return double
 */
double sphere_calc_theta_dot_dot(double r, double r_dot, double theta, double theta_dot, double phi_dot);

double sphere_calc_phi_dot(double Nphi, double Hbar, double theta, double mass, double r);

double sphere_calc_init_phi_dot(double K, double Hbar, double nphi, double mass, double r);

double sphere_calc_spc_case_r_dot_dot(double r, double theta_dot, double charge, double mass);

double sphere_calc_spc_case_theta_dot(double k, double Hbar, double mass, double r);

#endif // SPHERE_CALC_H