#include <stdio.h>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulation_runner/simulation_runner.h"
#include "utils/iterator.h"
#include "utils/types.h"

#define ORBIT_COUNT 16
struct file_record {
    long key;
    FILE *file;
};

long orbit_hash(struct electron_orbit orbit) {
    return orbit.principal | orbit.angular << 8 | orbit.magnetic << 16;
}

void *lookup(void *record_in, long key) {
    struct file_record *files = (struct file_record *)record_in;
    for (int i = 0; i < ORBIT_COUNT; i++) {
        if (files[i].key == key) {
            return files[i].file;
        }
    }
    return NULL;
}

FILE *create_record_file(const char *filename, enum sim_type sim_type) {
    FILE *record_file = fopen(filename, "w");
    fprintf(record_file, "t,r,r_dot,r_dot_dot,phi,phi_dot");

    if (sim_type == SPHERICAL || sim_type == REL_SPHERICAL) {
        fprintf(record_file, ",theta,theta_dot,theta_dot_dot");
    }

    if (sim_type == REL_SPHERICAL || sim_type == REL_POLAR) {
        fprintf(record_file, ",gamma,delta_phi");
    }

    fprintf(record_file, "\n");
    return record_file;
}

int main(void) {
    const float revolutions = 10;
    const scalar time_interval = 1e-7;
    const enum sim_type sim_type = REL_POLAR;
    const bool delta_psi_mode = false;
    const unsigned short record_interval = 30000;

    struct electron_orbit electrons[] = {
        {3, 1, 0, 0}
    };

    struct file_record orbit_files[ORBIT_COUNT];

    struct record_handler rh = {
        .record_in = &orbit_files,
        .record = &record2csv,
        .records_lookup = &lookup,
        .curr_records = 0,
    };

    struct atom atom = {
        .electrons = electrons,
        .electrons_count = sizeof(electrons) / sizeof(electrons[0]),
    };

    for (int i = 0; i < atom.electrons_count; i++) {
        char filename[100];
        sprintf(filename, "debug/d100/2d_rel_%d_%d_%d_d10%d.csv",
                atom.electrons[i].principal, atom.electrons[i].angular,
                atom.electrons[i].magnetic, i);

        long key = orbit_hash(atom.electrons[i]);
        orbit_files[i].key = key;
        orbit_files[i].file = create_record_file(filename, sim_type);
    }

    struct sim_ctx ctx = {
        .record_handler = &rh,
        .revolutions = revolutions,
        .delta_psi_mode = delta_psi_mode,
        .time_interval = time_interval,
        .record_interval = record_interval,
        .result = NULL,
        .active_orbits = atom.electrons_count,
    };

    struct simulator sim;

    init_simulation(&sim, atom, &ctx, sim_type);

    run_simulation(&sim);

    for (int i = 0; i < atom.electrons_count; i++) {
        fclose(orbit_files[i].file);
    }
    return 0;
}