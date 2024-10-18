#ifndef MACROS_H
#define MACROS_H

// sim_itr macros
#define DT(itr) (itr)->dt
#define DR(itr) (itr)->r
#define R_DOT(itr) (itr)->r_dot
#define R_DOT_DOT(itr) (itr)->r_dot_dot
#define PHI(itr) (itr)->phi
#define PHI_DOT(itr) (itr)->phi_dot
#define PHI_DOT_0(itr) (itr)->initial_phi_dot
#define DELTA_PHI(itr) (itr)->delta_phi
#define GAMMA(itr) (itr)->gamma
#define DELTA_PHI(itr) (itr)->delta_phi
#define THETA(itr) (itr)->theta
#define THETA_DOT(itr) (itr)->theta_dot
#define THETA_DOT_DOT(itr) (itr)->theta_dot_dot
#define EPSILON(itr) (itr)->epsilon

// config macros
#define TIME_INTERVAL(config) (config)->time_interval
#define HBAR(config) (config)->Hbar
#define MASS(config) (config)->electron_mass
#define CHARGE(config) (config)->electron_charge
#define LOG_P(config) (config)->log_p

// constants 
// TODO: move them to a constants.h file
#define BOHR_R 5.29177210903e-9
#define PI 3.14159265358979323846
#define _2_PI 6.28318530717958647692
#define C 29979245800

#endif // MACROS_H