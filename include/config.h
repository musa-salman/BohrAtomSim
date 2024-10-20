#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <utils/linked_list.h>
#include <utils/types.h>

struct config
{
    enum sim_type type; // indicator to the type of equations to use 0 use regular 1 use relativity
    bool itr_mode;

    // TODO what is delta_psi_mode?
    bool delta_psi_mode;
    int log_p; // the number of iterations to skip before logging
    size_t iters; // number of iteration to run
    long double revolutions;
    long double electron_charge; // electron charge value = 4.803e-10 esu
    long double electron_mass;   // electron max value = 9.109383e-28 g
    long double time_interval;   // time elapsed between each iteration
    
    struct linked_list *filter_list; // contains the exact orbits to simulate
    struct linked_list *log_files;   // contains the files to write
    char *timestamp;                 // the id of the test results
};

struct config *load_config(const char *config_path);

#endif // CONFIG_H