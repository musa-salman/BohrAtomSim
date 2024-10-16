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
 * @brief calcuates the analitcal ratio of the apocenter
 *
 *          CHI = (1 - (e^2 / (h_bar*C*energy_level))^2)
 *
 * @param h_bar
 * @param charge
 * @param r
 * @param energy_level
 * @return double
 */
long double calc_rel_chi(long double h_bar, long double charge, long double energy_level);

/**
 * @brief Calculates Rmin with relativty incorporated the starting point of the simulation
 *
 *                      RMIN = ( ( -B + (B^2 - A*C)^0.5 ) / A )
 * @param a
 * @param b
 * @param c
 * @return long double
 */
long double calc_rel_rmin(long double a, long double b, long double c);

/**
 * @brief Calculates rel_A to be used in calculating Rmin
 *
 *                  A = 2*m*W + (W^2 / C^2)
 *
 * @param C = 29979245800
 * @param mass = 9.109383e-28
 * @param w = calc_rel_W
 * @return long double
 */
long double calc_rel_A(long double mass, long double w);

/**
 * @brief Calculates rel_B to be used in calculating Rmin
 *
 *              B = m*e^2 + (W*e^2 / C^2)
 *
 * @param mass
 * @param charge
 * @param w = calc_rel_W
 * @return long double
 */
long double calc_rel_B(long double mass, long double charge, long double w);

/**
 * @brief Calculates rel_C to be used in calculating Rmin
 *
 *          C = -(k*hbar)^2 + (e^4 / C(speed of light)^2)
 *
 * @param l_sqr = (k*hbar)^2
 * @param charge
 * @return long double
 */
long double calc_rel_C(long double l_sqr, long double charge);

/**
 * @brief Calculates rel_W to be used in calculating A,B for Rmin
 *
 *              1+(W/(m*c^2)) = (1+ (alpha^2 / (n-k + (k^2 - alpha^2)^0.5 )^2 ))^-0.5
 *
 * @param energy_level
 * @param K
 * @param mass
 * @param alpha
 * @return long double
 */
long double calc_rel_w(long double energy_level, long double K, 
long double mass, long double alpha);

/**
 * @brief calculates an approximation of the finite structure constant ~ (1/137)
 *
 *                      alpha = (charge^2)/(C*hbar)
 *
 * @param charge
 * @param hbar
 * @return long double
 */
long double calc_alpha(long double charge, long double hbar);

#endif // EVAL_REL_H