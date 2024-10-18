#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <utils/linked_list.h>
#include <utils/reads.h>
#include <config.h>
#include <filter.h>

void m_block(struct linked_list *list, char **filter_lines, unsigned char principle, unsigned char angular)
{
    const char *curr_line = filter_lines[0];

    const signed char m = (unsigned char)atoi(curr_line);

    struct electron_orbit *orbit = malloc(sizeof(struct electron_orbit));

    *orbit = (struct electron_orbit){principle, angular, m};

    linked_list_append(list, orbit);

    return;
}

void k_block(enum sim_type type, struct linked_list *list, char **filter_lines, int line_count, int energy_level)
{
    int k = 0;
    for (int i = 0; i < line_count; i++)
    {
        const char *line = filter_lines[i];
        if (strstr(line, "K") != NULL)
        {
            k = atoi(line);
            if (type == POLAR || type == REL_POLAR)
            {
                struct electron_orbit *orbit = malloc(sizeof(struct electron_orbit));
                *orbit = (struct electron_orbit){energy_level, k, -1};
                linked_list_append(list, orbit);
                return;
            }
        }
        if (strstr(line, "{}") != NULL)
        { // if the block is empty simulate all

            if (type == SPIN || type == SPHERICAL || type == REL_SPHERICAL)
            {

                for (int m = 0; m <= k; m++)
                {

                    struct electron_orbit *orbit = malloc(sizeof(struct electron_orbit));
                    *orbit = (struct electron_orbit){energy_level, k, m};

                    linked_list_append(list, orbit);
                }
            }
            else
            {
                struct electron_orbit *orbit = malloc(sizeof(struct electron_orbit));
                *orbit = (struct electron_orbit){energy_level, k, -1};

                linked_list_append(list, orbit);
            }
            return;
        }
        else if (strstr(line, "M") != NULL)
        { // block not empty
            m_block(list, filter_lines + i, energy_level, k);
        }
        else if (strstr(line, "}") != NULL)
        {
            return;
        }
    }
}

void n_block(enum sim_type type, struct linked_list *list, char **filter_lines, int line_count)
{
    unsigned char principal = 0;

    for (int i = 0; i < line_count; i++)
    {
        const char *curr_line = filter_lines[i];
        if (strstr(curr_line, "N") != NULL)
        {
            principal = (unsigned char)atoi(curr_line);
            if (strstr(curr_line, "{}") != NULL)
            { // if the block is empty simulate all
                for (unsigned char k = 1; k <= principal; k++)
                {

                    if (type == SPIN || type == SPHERICAL || type == REL_SPHERICAL)
                    {

                        for (signed char m = 0; m <= k; m++)
                        {

                            struct electron_orbit *orbit = malloc(sizeof(struct electron_orbit));
                            *orbit = (struct electron_orbit){principal, k, m};
                            linked_list_append(list, orbit);
                        }
                    }
                    else
                    {

                        struct electron_orbit *orbit = malloc(sizeof(struct electron_orbit));
                        *orbit = (struct electron_orbit){principal, k, -1};
                        linked_list_append(list, orbit);
                    }
                }
            }
        }
        else if (strstr(curr_line, "K") != NULL)
        { // block not empty
            k_block(type, list, filter_lines + i, line_count - i, principal);
        }
    }
}

char *print_filter(void *data)
{
    const struct electron_orbit *orbit = (struct electron_orbit *)data;

    char *output = malloc(sizeof(char[100]));

    sprintf(output, "[%hi %hi %hi]", orbit->principal, orbit->angular, orbit->magnetic);

    return output;
}

struct linked_list *get_filter_list(enum sim_type type)
{
    FILE *filter_file = fopen(FILTER_PATH, "r");
    if(filter_file == NULL)
    {
        perror("ERROR opening filter file");
        exit(1);
    }
    struct linked_list *list = linked_list_new();

    int line_count = -1;

    char **filter_lines = read_lines(filter_file, &line_count);

    n_block(type, list, filter_lines, line_count);

    for (int i = 0; i < line_count; i++)
    {
        free(filter_lines[i]);
    }

    free(filter_lines);
    fclose(filter_file);

    // TODO why is this here?
    linked_list_print(list, &print_filter);
    return list;
}