#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <spherical/spherical_calc.h>

double sphere_calc_theta_min(double N_phi, double k)
{
    return asin(N_phi / k);
}

double sphere_calc_phi_dot(
    double N_phi, double Hbar, double theta,
    double mass, double r)
{

    double arg1 = N_phi * Hbar;

    double arg2 = sin(theta);
    arg2 *= arg2;
    arg2 *= mass * r * r;

    return arg1 / arg2;
}

double sphere_calc_r_dot_dot(
    double m, double r, double e,
    double k_sqr, double Hbar_sqr)
{

    double arg1, arg2;

    double r_sq = r * r;

    arg1 = k_sqr * Hbar_sqr;
    arg1 /= m * r_sq * r;

    arg2 = e * e;
    arg2 /= r_sq;

    arg1 = arg1 - arg2;

    arg1 /= m;

    return arg1;
}

double sphere_calc_theta_dot_dot(
    double r, double r_dot, double theta,
    double theta_dot, double phi_dot)
{

    double arg1 = sin(theta) * cos(theta);
    arg1 *= phi_dot * phi_dot;

    double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    return arg1 - arg2;
}

double sphere_calc_init_phi_dot(
    double K, double Hbar, double N_phi,
    double mass, double r)
{

    if (N_phi == 0)
        return 0;

    double arg1 = K * K * Hbar;

    double arg2 = N_phi * mass * r * r;

    return arg1 / arg2;
}

double sphere_calc_spc_case_r_dot_dot(
    double r, double theta_dot,
    double charge, double mass)
{
    double arg1 = r * theta_dot * theta_dot;

    double arg2 = charge * charge;

    arg2 /= mass * r * r;

    return arg1 - arg2;
}

double sphere_calc_spc_case_theta_dot(
    double k, double Hbar,
    double mass, double r)
{

    double arg1 = k * Hbar;

    double arg2 = r * r * mass;

    return arg1 / arg2;
}
