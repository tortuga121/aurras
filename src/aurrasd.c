#include "filters.h"
#include "utilities.h"

int main(int argc, char ** argv) {
    //if(argc < 3) perror_invalid_args();
    FILTERS fs = fill_filters(argv[1]);
    if(fs == NULL) return -1;
    print_filter(fs);
    return 0;
}

