#ifndef POLAR_SIM_REL_H
#define POLAR_SIM_REL_H

#include "config.h"

/**
 * @brief runs the simulation using the relativistic equations in CalcRel.h
 *
 * @param result_files array of files to to store the results
 * @param config the settings of the simulation
 */
void polar_sim_rel_ele(struct config *config);

#endif // POLAR_SIM_REL_H