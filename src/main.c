#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <polar/polar_sim.h>
#include <polar/polar_sim_rel.h>
#include <spherical/spherical.h>
#include <spin/spin.h>
#include <config.h>
#include <utils/utils.h>

int main(void)
{
    struct config *config = load_config(CONFIG_PATH);

    switch (config->type)
    {
    case POLAR:
        polar_sim_ele(config);
        break;
    case REL_POLAR:
        polar_sim_rel_ele(config);
        break;
    case SPHERICAL:
        spherical_sim_ele(config);
        break;
    case REL_SPHERICAL:
        spherical_sim_rel_ele(config); 
        break;
    case SPIN:
        spin_sim_ele(config);
        break;
    case REL_SPIN:
        // TODO: implement
        break;
    }

    free(config->log_files);
    free(config->filter_list);
    free(config->timestamp);
    free(config);

    return 0;
}
