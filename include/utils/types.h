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

// simulation iteration to hold the current and next iteration values
struct sim_itr
{
    long double r;         // iteration distance
    long double r_dot;     // iteration movement speed
    long double r_dot_dot; // iteration movement acceleration

    long double phi;       // iteration angle
    long double delta_phi; // iteration angle of the perihelion
    long double initial_phi_dot;
    long double phi_dot; // iteration angular speed

    long double theta;
    long double theta_dot;
    long double theta_dot_dot;

    long double dt;    // iteration time
    long double gamma; // iteration rel mass mult
    long double epsilon;
};

#endif // TYPES_H