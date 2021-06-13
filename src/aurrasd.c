#include "filters.h"
#include "utilities.h"
char** divide_command(char* command) {

    char** params = malloc(sizeof(char*) * 16);
    // test if malloc failed
    if (params == NULL) perror("malloc failed");
    //initialize params
    for (int i = 0; i < MAX_ARGS; i++)
        params[i] = NULL;

    for (int i = 0; i < MAX_ARGS && command; i++) {
        char* arg = strsep(&command, " \n");
        if (!(arg && *arg)) continue;  // in case of empty string
        params[i] = strdup(arg);
    }
    return params;
}
int exec_status(FILTERS fs) {
    int fd = open(SERVER_TO_CLIENT,O_WRONLY,0777);
    if(fd == -1 && errno != EEXIST){ 
        perror(SERVER_TO_CLIENT);
        return -1;
    }
    char* out = status(fs);
    write(fd,out,strlen(out));
    close(fd);
    return 0;
}

int exec_command(char* command, FILTERS fs) {
    char** args = divide_command(command);
    if(!args) return -1;
    if(*args && !strcmp(args[0],"status")) {
        exec_status(fs);
    }
    return 0;
}
int main(int argc, char** argv) {
    if (argc < 3) perror_invalid_args();
    FILTERS fs = fill_filters(argv[1]);
    if (fs == NULL) return -1;
    int fifo_ret = mkfifo(SERVER_TO_CLIENT,0666);
    if (fifo_ret == -1 && errno != EEXIST) {
        perror(SERVER_TO_CLIENT);
        return -1;
    }
    while (1) {
        int fd = open(CLIENT_TO_SERVER, O_RDONLY, 0666);
        char command[COMMAND_SIZE];
        ssize_t bytes = read(fd, command, COMMAND_SIZE);
        if(bytes > 0) write(1,command,strlen(command));
        if (bytes > 0)
            exec_command(command,fs);
    }
    return 0;
}
