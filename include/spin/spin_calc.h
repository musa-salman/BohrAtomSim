#ifndef SPIN_CALC_H
#define SPIN_CALC_H

long double spin_calc_r_dot_dot(long double r, long double theta, long double theta_dot, long double Phi_dot_0, long double epsilon, long double mass, long double charge);

long double spin_calc_theta_dot_dot(long double r, long double r_dot, long double theta, long double theta_dot, long double phi_dot_0, long double epsilon);

long double spin_calc_phi_dot(long double phi_dot_0, long double epsilon);

long double spin_calc_phi_dot_0(long double r, long double mass, long double nphi, long double hbar, long double theta);

long double spin_calc_epsilon(long double r, long double mass, long double charge, long double theta, long double nphi);

#endif // SPIN_CALC_H