#include "filters.h"

filter line_to_filter(char* line) {
    filter f = malloc(sizeof(struct fltr));
    f->name = strdup(strsep(&line, " "));
    char path[LINE_SIZE] = "bin/aurrasd-filters/";
    f->path = strdup(strcat(path, strsep(&line, " ")));
    f->used = 0;
    f->max = (int)strtol(line, NULL, 10);
    if (f->name && f->name[0] && f->path && f->path[0]) return f;
    return NULL;
}

int add_filter(FILTERS fs, filter f) {
    if (fs->size >= fs->max_size && f) return -1;
    fs->fltr[fs->size] = f;
    fs->size++;
    return 0;
}

FILTERS init_filters(int n_filters) {
    FILTERS fs = malloc(sizeof(struct fltrs));
    fs->fltr = malloc(sizeof(struct fltr) * n_filters);
    fs->size = 0;
    fs->max_size = n_filters;
    // initializing with null
    for (int i = 0; i < n_filters; i++) fs->fltr[i] = NULL;
    return fs;
}

FILTERS fill_filters(char* config_file_path) {
    //opening config file
    int fd = open(config_file_path, O_RDONLY, 0666);
    if (fd == -1) {
        perror(config_file_path);
        return NULL;
    }

    // count number of filters
    int n_filters = 0;
    char c;
    while (read(fd, &c, 1) == 1)
        if (c == '\n') n_filters++;

    //go to beggingin of file
    lseek(fd, 0, SEEK_SET);

    //init filters
    FILTERS fs = init_filters(n_filters);

    // read filters from file and add
    char* line = malloc(LINE_SIZE);
    read(fd, line, LINE_SIZE);
    for (int i = 0; i < n_filters; i++) {
        filter f = line_to_filter(strsep(&line, "\n\0"));
        add_filter(fs, f);
    }
    return fs;
}
// returns string of the filter status
char* filter_status(filter f) {
    char line[LINE_SIZE] = "";
    strcat(line, "filter ");
    strcat(line, f->name);
    strcat(line, ":");
    strcat(line, " ");
    char number[20];
    sprintf(number, "%d/%d ", f->used, f->max);
    strcat(line, number);
    strcat(line, "running/max\n");
    return strdup(line);
}
// returns string of the filters status
char* status(FILTERS fs) {
    char line[2048] = "";
    for (int i = 0; i < fs->size; i++) {
        char* fstatus = filter_status(fs->fltr[i]);
        strcat(line, fstatus);
    }
    return strdup(line);
}
//check if filter is available
int is_available(filter f) {
    if (f == NULL) return 0;
    return f->used < f->max;
}
filter find_filter(char* filter_name, FILTERS fs) {
    for (int i = 0; i < fs->size; i++)
        if (!strcmp(fs->fltr[i]->name, filter_name))
            return fs->fltr[i];

    write(1, filter_name, strlen(filter_name));
    return NULL;
}
int can_transform(FILTERS fs, char** filters) {
    for (int i = 0; i < fs->size && filters[i]; i++)
        if (!is_available(find_filter(filters[i], fs)))
            return 0;
    return 1;
}
void ocup_filter(FILTERS fs, char* f_name) {
    filter f = find_filter(f_name, fs);
    if (f->used < f->max) f->used = f->used + 1;
    //printf("opcup %s %d\n",f->name,f->used);
}
void free_filter(FILTERS fs, char* f_name) {
    filter f = find_filter(f_name, fs);
    if (f->used > 0) f->used = f->used - 1;
    //printf("free %s %d\n",f->name,f->used);
}