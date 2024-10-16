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
    double dt;        // iteration time
    double dr;        // iteration distance
    double r_dot;     // iteration movement speed
    double r_dot_dot; // iteration movement acceleration
    double phi;       // iteration angle
    double phi_dot;   // iteration angular speed

    // TODO what is phi_dot_0?
    double phi_dot_0;
    double gamma;     // iteration rel mass mult
    double delta_phi; // iteration angle of the perihelion
    double theta;
    double theta_dot;
    double theta_dot_dot;
    double epsilon;
};

#endif // TYPES_H