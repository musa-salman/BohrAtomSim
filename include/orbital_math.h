#ifndef ORBITAL_MATH_H
#define ORBITAL_MATH_H


/**
    Calculates r_dot_dot "acceleration" of and electron
    where

        m*r_dot_dot = ((l^2)/(m*(r^3)) - (e^2)/(r^2)
        l = k * H_BAR
        r_dot_dot = (k^2 * H_BAR_SQR / (m*r^3) - e^2 / (r^2)) / m
*/
long double compute_r_dot_dot(long double mass, long double r, long double charge, long double k);

#endif // ORBITAL_MATH_H