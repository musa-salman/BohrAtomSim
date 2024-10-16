#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <polar/polar_calc.h>
#include <utils/macros.h>

long double calc_phi_dot(long double l, long double mass, long double r)
{
    return l / (mass * r * r);
}

long double calc_R_dot_dot(long double mass, long double r, long double charge,
                           long double k_sqr, long double Hbar_sqr)
{
    return ((k_sqr * Hbar_sqr) / (mass * r * r * r)) - (charge * charge / (r * r));
}

long double *calc_rmin_rmax(long double n, long double k)
{
    long double a = n * n;
    long double b = k / n;
    b *= a;

    long double dis = a * a;
    dis -= b * b;

    dis = sqrtl(dis);

    long double *results = malloc(sizeof(*results) * 2);
    results[0] = results[1] = a;
    results[0] -= dis;
    results[1] += dis;

    results[0] *= BOHR_R;
    results[1] *= BOHR_R;

    return results;
}