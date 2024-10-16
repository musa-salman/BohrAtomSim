#ifndef EVAL_H
#define EVAL_H

/*
*************************************************************************************************************
*** All calculations are done according to Atomic Structure And Spectral Lines Vol. I by Arnold Sommerfeld ***
*************************************************************************************************************
*/

/**
    Calculates R_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = (l^2)/(m*(r^3)) - (e^2)/(r^2)

    @param double m = 9.109383e-28 double e = 4.803 e-10 l_sqr = Calc_l_sqr r = r of the current iteration
    @return double r_dot_dot

*/
long double calc_R_dot_dot(long double m, long double r, long double e, long double k_sqr, long double Hbar_sqr);

/**

    Calculates the angel change rate of the electron movement
    where

        phi_dot = L / m*(r^2)

    @param double m = 9.109383e-28 double e = 4.803 e-10  r = r of the current iteration
    @return double phi_dot


*/
long double calc_phi_dot(long double l, long double m, long double r);

/**
    Calculates the R_max and the R_min for a given electron orbit
    where

        (1) r_min + r_max = 2*a
        (2) r_min*max = b^2

        where a = n^2 * BOHR_R
        and   b = k/n * a

    @param double n double k
    @return double result[2] containing the r_min at index 0 and r_max at index 1

*/
long double *calc_rmin_rmax(long double n, long double k);

#endif // EVAL_H
