#ifndef TYPES_H
#define TYPES_H

enum sim_type
{
    POLAR,
    REL_POLAR,
    SPHERICAL,
    REL_SPHERICAL,
    SPIN,
    REL_SPIN
};

struct orbit
{
    unsigned int n : 16;
    unsigned int k : 16;
    unsigned int m : 16;
};

// simulation iteration to hold the current and next iteration values
struct sim_itr
{
    long double dt;        // iteration time
    long double dr;        // iteration distance
    long double r_dot;     // iteration movement speed
    long double r_dot_dot; // iteration movement acceleration
    long double phi;       // iteration angle
    long double phi_dot;   // iteration angular speed

    // TODO what is phi_dot_0?
    long double phi_dot_0;
    long double gamma;     // iteration rel mass mult
    long double delta_phi; // iteration angle of the perihelion
    long double theta;
    long double theta_dot;
    long double theta_dot_dot;
    long double epsilon;
};

#endif // TYPES_H