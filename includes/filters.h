#ifndef FILTERS_H
#define FILTERS_H
#include "utilities.h"
typedef struct fltr {
    char* name;  // Name of the filter
    char* path;  // Executable name
    int used;    // Number of processes using the filter
    int max;     // Max number of processes using the filter
} * filter;

typedef struct fltrs {
    filter* fltr;  // Filters array
    int size;      // Number of filters
    int max_size;  // Max number of filters
} * FILTERS;
FILTERS fill_filters(char* config_file_path);
char* status(FILTERS fs);
int can_transform(FILTERS fs, char **filters);
#endif
