#ifndef SPHERE_CALC_REL_H
#define SPHERE_CALC_REL_H

double rel_sphere_calc_theta_min(double Nphi, double k);

double rel_sphere_calc_r_dot_dot(double k_sqr, double Hbar_sqr, double mass, double gamma, double r, double charge, double r_dot);

double rel_sphere_calc_phi_dot(double Nphi, double Hbar, double theta, double mass, double r, double gamma);

double rel_sphere_calc_theta_dot_dot(double r, double r_dot, double theta, double theta_dot, double phi_dot, double charge, double mass, double gamma);

double rel_sphere_calc_gamma(double k, double Hbar, double mass, double r, double r_dot);

double rel_sphere_calc_spc_case_r_dot_dot(double r, double theta_dot, double charge, double mass);

double rel_sphere_calc_spc_case_theta_dot(double k, double Hbar, double mass, double r, double gamma);

double rel_sphere_calc_delta_phi(double *curr_max_vec, double *prev_max_vec, double rMax);

double *stoc(double roh, double phi, double theta);

double dot_product(const double *v1, const double *v2, size_t size);

#endif // SPHERE_CALC_REL_H