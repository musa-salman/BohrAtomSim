#ifndef EVAL_REL_H
#define EVAL_REL_H

/*
*************************************************************************************************************
*** All calculations are done according to Atomic Structure And Spectral Lines Vol. I by Arnold Sommerfeld ***
*************************************************************************************************************
*/

/**
 * @brief Calculates phi dot with relativity incorporated  "angular speed" of and electron
 *
 *                      phi = l / (gamma * mass * r^2)
 *
 * @param l  = k*hbar
 * @param gamma
 * @param r
 * @param m
 * @return double
 */
long double calc_rel_phi_dot(long double l, long double gamma, long double r, long double m);

/**
 * @brief Calculates gamma  "change of the mass" of and electron
 *
 * @param l = k*hbar
 * @param mass
 * @param r
 * @param r_dot
 * @return double
 */
long double calc_rel_gamma(long double l, long double mass, long double r, long double r_dot);

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
long double calc_rel_r_dot_dot(long double l_sqr, long double mass, long double gamma, long double r, long double charge, long double r_dot);

/**
 * @brief calculates the analytical ratio of the apocenter
 *
 *          CHI = (1 - (e^2 / (h_bar*C*energy_level))^2)
 *
 * @param charge
 * @param angular
 * @return double
 */
long double calc_rel_chi(long double charge, long double angular);

#endif // EVAL_REL_H