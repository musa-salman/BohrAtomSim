#ifndef EVAL_H
#define EVAL_H

struct radial_bounds
{
    long double r_min;
    long double r_max;
};

/*
*************************************************************************************************************
*** All calculations are done according to Atomic Structure And Spectral Lines Vol. I by Arnold Sommerfeld ***
*************************************************************************************************************
*/

/**
    Calculates R_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = ((l^2)/(m*(r^3)) - (e^2)/(r^2)

        r_dot_dot = (k^2 * H_BAR_SQR / (m*r^3) - e^2 / (r^2)) / m

    @param double mass = 9.109383e-28 double charge = 4.803e-10 l_sqr = Calc_l_sqr r = r of the current iteration
    @return double r_dot_dot

*/
long double compute_r_dot_dot(long double mass, long double r, long double charge, long double k);

/**

    Calculates the angel change rate of the electron movement
    where

        phi_dot = L / mass*(r^2)

    @param double m = 9.109383e-28 double charge = 4.803 charge-10  r = r of the current iteration
    @return double phi_dot


*/
long double compute_phi_dot(long double l, long double mass, long double r);

/**
    Calculates the R_max and the R_min for a given electron orbit
    where

        (1) r_min + r_max = 2*a
        (2) r_min*max = b^2

        where a = n^2 * BOHR_R
        and   b = k/n * a
*/
struct radial_bounds *compute_radial_limits(long double energy_level, long double k);

#endif // EVAL_H
