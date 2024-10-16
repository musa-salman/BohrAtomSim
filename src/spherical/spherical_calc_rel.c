#include <math.h>
#include <stdlib.h>

#include <utils/types.h>
#include <utils/macros.h>
#include <spherical/spherical_calc_rel.h>

double rel_sphere_calc_r_dot_dot(double k_sqr, double Hbar_sqr, double mass,
                                 double gamma, double r, double charge,
                                 double r_dot)
{
    double arg1;
    double arg2;
    double arg3;

    double r_sq = r * r;

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

double rel_sphere_calc_phi_dot(double N_phi, double Hbar, double theta,
                               double mass, double r, double gamma)
{

    double arg1 = N_phi * Hbar;

    double arg2 = sin(theta);
    arg2 *= arg2;
    arg2 *= gamma * mass * r * r;

    return arg1 / arg2;
}

double rel_sphere_calc_theta_dot_dot(
    double r, double r_dot, double theta,
    double theta_dot, double phi_dot, double charge,
    double mass, double gamma)
{
    double arg1 = sin(theta) * cos(theta);
    arg1 *= phi_dot * phi_dot;

    double arg2 = r_dot / r;
    arg2 *= 2 * theta_dot;

    double arg3 = gamma * mass * C * C * r;
    arg3 = (charge * charge) / arg3;
    arg3 = 1 - arg3;
    arg2 *= arg3;

    return arg1 - arg2;
}

double rel_sphere_calc_gamma(
    double k, double Hbar, double mass,
    double r, double r_dot)
{
    double arg1 = C;
    arg1 *= mass * r;
    arg1 = (k * Hbar) / arg1;
    arg1 *= arg1;
    arg1++;

    double arg2 = C;
    arg2 = r_dot / arg2;
    arg2 *= arg2;
    arg2 = 1 - arg2;

    arg1 = sqrt(arg1 / arg2);

    return arg1;
}

double rel_sphere_calc_theta_min(double N_phi, double k)
{
    return asin(N_phi / k);
}

double rel_sphere_calc_spc_case_r_dot_dot(
    double r, double theta_dot, double charge,
    double mass, double gamma)
{

    double arg1 = r * theta_dot * theta_dot;

    double arg2 = charge * charge;

    arg2 /= (mass * r * r);

    return arg1 - arg2;
}

double rel_sphere_calc_spc_case_theta_dot(
    double k, double Hbar, double mass,
    double r, double gamma)
{

    double arg1 = k * Hbar;

    double arg2 = r * r * mass * gamma;

    return arg1 / arg2;
}

double rel_sphere_calc_deltaPhi(double *currMaxVec, double *prevMaxVec, double rMax)
{

    double arg1 = dot_product(currMaxVec, prevMaxVec, 3);

    // printf("scalar Mult = %e\n",arg1);
    double rmax_curr = sqrt(currMaxVec[0] * currMaxVec[0] + currMaxVec[1] * currMaxVec[1] + currMaxVec[2] * currMaxVec[2]);
    double rmax_prev = sqrt(prevMaxVec[0] * prevMaxVec[0] + prevMaxVec[1] * prevMaxVec[1] + prevMaxVec[2] * prevMaxVec[2]);
    arg1 /= (rmax_curr * rmax_prev);
    // printf("rmax_curr %.*e , rmax_prev %.*e \n",rmax_curr,DECIMAL_DIG,rmax_prev,DECIMAL_DIG);
    // printf("cos(psi) = %.*e\n",arg1,DECIMAL_DIG);

    return acos(arg1);
}

double *stoc(double roh, double phi, double theta)
{

    double *cords = (double *)malloc(sizeof(double) * 3);

    double x = roh * cos(phi) * sin(theta);
    double y = roh * sin(phi) * sin(theta);
    double z = roh * cos(theta);

    cords[0] = x;
    cords[1] = y;
    cords[2] = z;

    return cords;
}

double dot_product(double *v1, double *v2, int size)
{
    double sum = 0;
    for (int i = 0; i < size; i++)
    {

        sum += v1[i] * v2[i];
    }
    return sum;
}