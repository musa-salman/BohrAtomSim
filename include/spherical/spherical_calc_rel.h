#ifndef SPHERE_CALC_REL_H
#define SPHERE_CALC_REL_H

long double rel_sphere_calc_theta_min(long double N_phi, long double k);

long double rel_sphere_calc_r_dot_dot(long double k_sqr, long double Hbar_sqr, long double mass,
                                      long double gamma, long double r, long double charge, long double r_dot);

long double rel_sphere_calc_phi_dot(long double N_phi, long double Hbar, long double theta,
                                    long double mass, long double r, long double gamma);

typedef struct spherical_calc_rel_params
{
    long double r;
    long double r_dot;
    long double theta;
    long double theta_dot;
    long double phi_dot;
    long double charge;
    long double mass;
    long double gamma;
} spherical_calc_rel_params;

long double rel_sphere_calc_theta_dot_dot(struct spherical_calc_rel_params params);

long double rel_sphere_calc_gamma(long double k, long double Hbar, long double mass, long double r, long double r_dot);

long double rel_sphere_calc_spc_case_r_dot_dot(long double r, long double theta_dot, long double charge, long double mass);

long double rel_sphere_calc_spc_case_theta_dot(long double k, long double Hbar, long double mass, long double r, long double gamma);

long double rel_sphere_calc_delta_phi(const long double *curr_max_vec, const long double *prev_max_vec, long double rMax);

long double *stoc(long double roh, long double phi, long double theta);

long double dot_product(const long double *v1, const long double *v2, size_t size);

#endif // SPHERE_CALC_REL_H