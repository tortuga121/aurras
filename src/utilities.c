#include "utilities.h"

void perror_invalid_args() {
    write(STDOUT_FILENO,"Invalid args!\n",strlen("Invalid args!\n"));
}


