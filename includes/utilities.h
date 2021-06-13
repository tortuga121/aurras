#ifndef UTILITIES_H
#define UTILITIES_H
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "utilities.h"

#include <stdio.h>
#define LINE_SIZE 1024
#define STATUS_SIZE 2048
#define COMMAND_SIZE 1024
#define MAX_ARGS 16
#define SERVER_TO_CLIENT "tmp/server_to_client_fifo"
#define CLIENT_TO_SERVER "tmp/client_to_server_fifo"

ssize_t readln(int fd, char *line, size_t size);

void perror_invalid_args();

#endif
