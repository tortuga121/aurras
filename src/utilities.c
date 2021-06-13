#include "utilities.h"

void perrror_invalid_args() {
    write(STDOUT_FILENO,"Invalid args!\n",strlen("Invalid args!\n"));
}