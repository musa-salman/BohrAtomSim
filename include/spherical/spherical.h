#ifndef SPHERICAL_H
#define SPHERICAL_H

#include "config.h"

/**
 * @brief runs the simulation using the non-relativistic equations in SphericalCalc.h
 *
 * @param result_files array of files to to store the results
 * @param config the settings of the simulation
 */
void spherical_sim_ele(struct config *config);

/**
 * @brief writes the current values from a given simItr to a given FILE*
 *
 * @param result_f
 * @param itr
 */
void spherical_sim_rel_ele(struct config *config);

#endif // SPHERICAL_H