#ifndef FILTER_H
#define FILTER_H

#include <utils/linked_list.h>

#define FILTER_PATH "filter.ini"

struct linked_list *get_filter_list(enum sim_type type);

#endif // FILTER_H