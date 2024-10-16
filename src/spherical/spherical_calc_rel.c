#include <math.h>
#include <stdlib.h>

#include <utils/types.h>
#include <utils/macros.h>
#include <spherical/spherical_calc_rel.h>

long double rel_sphere_calc_r_dot_dot(long double k_sqr, long double Hbar_sqr, long double mass,
                                 long double gamma, long double r, long double charge,
                                 long double r_dot)
{
    long double arg1;
    long double arg2;
    long double arg3;

    long double r_sq = r * r;

    arg1 = k_sqr * Hbar_sqr;
    arg1 /= gamma * mass * r_sq * r;

    arg2 = charge * charge;
    arg2 /= r_sq;

    arg3 = r_dot / C;
    arg3 *= arg3;
    arg3 = 1 - arg3;

    arg2 *= arg3;

    arg1 = arg1 - arg2;

    arg1 /= gamma * mass;

    return arg1;
}

long double rel_sphere_calc_phi_dot(long double N_phi, long double Hbar, long double theta,
                               long double mass, long double r, long double gamma)
{

    long double arg1 = N_phi * Hbar;

    long double arg2 = sinl(theta);
    arg2 *= arg2;
    arg2 *= gamma * mass * r * r;

    return arg1 / arg2;
}

long double rel_sphere_calc_theta_dot_dot(struct spherical_calc_rel_params params)
{
    long double r = params.r;
    long double r_dot = params.r_dot;
    long double theta = params.theta;
    long double theta_dot = params.theta_dot;
    long double phi_dot = params.phi_dot;
    long double charge = params.charge;
    long double mass = params.mass;
    long double gamma = params.gamma;

    long double arg1 = sinl(theta) * cosl(theta);
    arg1 *= phi_dot * phi_dot;

    long double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    long double arg3 = gamma * mass * C * C * r;
    arg3 = (charge * charge) / arg3;
    arg3 = 1 - arg3;
    arg2 *= arg3;

    return arg1 - arg2;
}

long double rel_sphere_calc_gamma(
    long double k, long double Hbar, long double mass,
    long double r, long double r_dot)
{
    long double arg1 = C;
    arg1 *= mass * r;
    arg1 = (k * Hbar) / arg1;
    arg1 *= arg1;
    arg1++;

    long double arg2 = C;
    arg2 = r_dot / arg2;
    arg2 *= arg2;
    arg2 = 1 - arg2;

    arg1 = sqrtl(arg1 / arg2);

    return arg1;
}

long double rel_sphere_calc_theta_min(long double N_phi, long double k)
{
    return asinl(N_phi / k);
}

long double rel_sphere_calc_spc_case_r_dot_dot(
    long double r, long double theta_dot, long double charge,
    long double mass)
{

    long double arg1 = r * theta_dot * theta_dot;

    long double arg2 = charge * charge;

    arg2 /= (mass * r * r);

    return arg1 - arg2;
}

long double rel_sphere_calc_spc_case_theta_dot(
    long double k, long double Hbar, long double mass,
    long double r, long double gamma)
{

    long double arg1 = k * Hbar;

    long double arg2 = r * r * mass * gamma;

    return arg1 / arg2;
}

long double rel_sphere_calc_delta_phi(const long double *curr_max_vec, const long double *prev_max_vec)
{

    long double arg1 = dot_product(curr_max_vec, prev_max_vec, 3);

    long double rmax_curr = sqrtl(dot_product(curr_max_vec, curr_max_vec, 3));
    long double rmax_prev = sqrtl(dot_product(prev_max_vec, prev_max_vec, 3));
    arg1 /= (rmax_curr * rmax_prev);

    return acosl(arg1);
}

long double *stoc(long double roh, long double phi, long double theta)
{

    long double *cords = malloc(sizeof(*cords) * 3);

    long double x = roh * cosl(phi) * sinl(theta);
    long double y = roh * sinl(phi) * sinl(theta);
    long double z = roh * cosl(theta);

    cords[0] = x;
    cords[1] = y;
    cords[2] = z;

    return cords;
}

long double dot_product(const long double *v1, const long double *v2, size_t size)
{
    long double sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += v1[i] * v2[i];
    }
    return sum;
}