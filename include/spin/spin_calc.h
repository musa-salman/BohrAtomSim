#ifndef SPIN_CALC_H
#define SPIN_CALC_H

double spin_calc_r_dot_dot(double r, double theta, double theta_dot, double Phi_dot_0, double epsilon, double mass, double charge);

double spin_calc_theta_dot_dot(double r, double r_dot, double theta, double theta_dot, double phi_dot_0, double epsilon);

double spin_calc_phi_dot(double phi_dot_0, double epsilon);

double spin_calc_phi_dot_0(double r, double mass, double nphi, double hbar, double theta);

double spin_calc_epsilon(double r, double mass, double charge, double theta, double nphi);

#endif // SPIN_CALC_H