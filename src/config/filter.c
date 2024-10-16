#include <utils/linked_list.h>
#include <config.h>
#include <filter.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void m_block(enum sim_type type, struct linked_list *list, char **filter_lines, int n, int k)
{
    char *curr_line = filter_lines[0];

    int m = atoi(curr_line);

    struct orbit *orbit = malloc(sizeof(struct orbit));

    *orbit = (struct orbit){n, k, m};

    linked_list_append(list, orbit);

    return;
}

void k_block(enum sim_type type, struct linked_list *list, char **filter_lines, int line_count, int n)
{

    int k = 0;
    for (int i = 0; i < line_count; i++)
    {
        char *line = filter_lines[i];
        if (strstr(line, "K") != NULL)
        {
            k = atoi(line);
            if (type == POLAR || type == REL_POLAR)
            {
                struct orbit *orbit = malloc(sizeof(struct orbit));
                *orbit = (struct orbit){n, k, -1};
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

                    struct orbit *orbit = malloc(sizeof(struct orbit));
                    *orbit = (struct orbit){n, k, m};

                    linked_list_append(list, orbit);
                }
            }
            else
            {
                struct orbit *orbit = malloc(sizeof(struct orbit));
                *orbit = (struct orbit){n, k, -1};

                linked_list_append(list, orbit);
            }
            return;
        }
        else if (strstr(line, "M") != NULL)
        { // block not empty

            m_block(type, list, filter_lines + i, n, k);
        }
        else if (strstr(line, "}") != NULL)
        {
            return;
        }
    }
}

void n_block(enum sim_type type, struct linked_list *list, char **filter_lines, int line_count)
{
    int n = 0;

    for (int i = 0; i < line_count; i++)
    {
        char *curr_line = filter_lines[i];
        if (strstr(curr_line, "N") != NULL)
        {
            n = atoi(curr_line);
            if (strstr(curr_line, "{}") != NULL)
            { // if the block is empty simulate all
                for (int k = 1; k <= n; k++)
                {

                    if (type == SPIN || type == SPHERICAL || type == REL_SPHERICAL)
                    {

                        for (int m = 0; m <= k; m++)
                        {

                            struct orbit *orbit = malloc(sizeof(struct orbit));

                            *orbit = (struct orbit){n, k, m};
                        }
                    }
                    else
                    {

                        struct orbit *orbit = malloc(sizeof(struct orbit));
                        *orbit = (struct orbit){n, k, -1};
                        linked_list_append(list, orbit);
                    }
                }
            }
        }
        else if (strstr(curr_line, "K") != NULL)
        { // block not empty
            k_block(type, list, filter_lines + i, line_count - i, n);
        }
    }
}

char *print_filter(void *data)
{
    struct orbit *orbit = (struct orbit *)data;

    char output[20];

    sprintf(output, "[%hi %hi %hi]", orbit->n, orbit->k, orbit->m);

    return output;
}

struct linked_list *get_filter_list(enum sim_type type)
{

    struct linked_list *list = linked_list_new();

    FILE *filter_file = fopen(FILTER_PATH, "r");

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
    linked_list_print(list, print_filter);
    return list;
}