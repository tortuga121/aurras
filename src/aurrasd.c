#include "filters.h"
#include "utilities.h"
char** divide_command(char* command) {
    char** params = malloc(sizeof(char*) * 16);
    // test if malloc failed
    if (params == NULL) {
        perror("malloc failed");
        return NULL;
    }
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
    int fd = open(SERVER_TO_CLIENT, O_WRONLY, 0777);
    if (fd == -1 && errno != EEXIST) {
        perror(SERVER_TO_CLIENT);
        return -1;
    }
    char* out = status(fs);
    write(fd, out, strlen(out));
    close(fd);
    return 0;
}

int exec_transform(char** args, FILTERS fs) {
    if (!can_transform(fs, args + 2)) return -1;
    char* audio_in = args[0];   // sample name
    char* audio_out = args[1];  // sample result name
    //open sample
    int f_in = open(audio_in, O_RDONLY, 0666);
    if (f_in == -1) {
        perror(audio_in);
        return -1;
    }
    // open result
    int f_out = open(audio_out, O_CREAT | O_WRONLY, 0666);
    if (f_in == -1) {
        perror(audio_out);
        return -1;
    }
    dup2(f_in,0);
    dup2(f_out,1);
    int fd[2];
    int status;
    int i;
    for (i = 2; i < MAX_ARGS && args[i + 1]; i++) {
        if (pipe(fd) == -1) {
            perror("pipe");
            return -1;
        }
        int pid_child = fork();
        if (pid_child == 0) {
            dup2(fd[1], 1);
            close(fd[1]);
            close(fd[0]);
            if (execl(fs->fltr[i]->path, fs->fltr[i]->path, NULL)) {
                perror("Filter Failed");
                return -1;
            }

        } else {
            dup2(fd[0], 0);
            close(fd[0]);
            close(fd[1]);
            waitpid(pid_child, &status, 0);
            if (WEXITSTATUS(stat) == -1) return -1;
        }
    }
    if (fork() == 0) {
        if (execl(fs->fltr[i]->path, fs->fltr[i]->path, NULL)) {
            perror("Filter Failed");
            return -1;
        }
    }
    return 0;
}
int exec_command(char* command, FILTERS fs) {
    char** args = divide_command(command);
    if (!args) return -1;
    if (*args && !strcmp(args[0], "status")) {
        exec_status(fs);
    } else if (*args && !strcmp(args[0], "transform"))
        if (exec_transform(args + 1, fs))
            write(1, "transform failed!\n", strlen("transform failed!\n"));
    return 0;
}
int main(int argc, char** argv) {
    if (argc < 3) perror_invalid_args();

    FILTERS fs = fill_filters(argv[1]);
    if (fs == NULL) return -1;

    int fifo_ret = mkfifo(SERVER_TO_CLIENT, 0666);
    if (fifo_ret == -1 && errno != EEXIST) {
        perror(SERVER_TO_CLIENT);
        return -1;
    }
    while (1) {
        // open fifo to read from client
        int fd = open(CLIENT_TO_SERVER, O_RDONLY, 0666);
        char command[COMMAND_SIZE];
        ssize_t bytes = read(fd, command, COMMAND_SIZE);
        if (bytes > 0)
            exec_command(command, fs);
    }
    return 0;
}
