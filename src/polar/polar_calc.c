#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <polar/polar_calc.h>
#include <utils/macros.h>

long double calc_phi_dot(long double l, long double m, long double r)
{
    return l / (m * r * r);
}

long double calc_R_dot_dot(long double m, long double r, long double e, long double k_sqr, long double Hbar_sqr)
{
    long double arg1;
    long double arg2;
    long double r_sq = r * r;

    arg1 = k_sqr * Hbar_sqr;
    arg1 /= m * r_sq * r;

    arg2 = e * e;
    arg2 /= r_sq;

    arg1 = arg1 - arg2;

    arg1 /= m;

    return arg1;
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