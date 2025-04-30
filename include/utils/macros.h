#ifndef MACROS_H
#define MACROS_H

// sim_itr macros
#define DT(itr) (itr)->dt
#define RHO(itr) (itr)->r
#define R_DOT(itr) (itr)->r_dot
#define R_DOT_DOT(itr) (itr)->r_ddot
#define PHI(itr) (itr)->phi
#define PHI_DOT(itr) (itr)->phi_dot
#define PHI_DOT_0(itr) (itr)->initial_phi_dot
#define DELTA_PHI(itr) (itr)->delta_phi
#define GAMMA(itr) (itr)->gamma
#define DELTA_PHI(itr) (itr)->delta_phi
#define THETA(itr) (itr)->theta
#define THETA_DOT(itr) (itr)->theta_dot
#define THETA_DOT_DOT(itr) (itr)->theta_ddot
#define EPSILON(itr) (itr)->epsilon

// atom macros
#define TIME_INTERVAL(ctx) (ctx)->time_interval

#endif // MACROS_H