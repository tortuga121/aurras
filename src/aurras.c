#include "utilities.h"
void tranform_error(int signal) {
    write(1,"transform failed\n",strlen("transform failed\n"));
}
void transform_sucess(int signal) {
    write(1,"done\n",strlen("done\n"));
}
int main(int argc, char **argv) {
    //
    signal(SIGUSR1,tranform_error);
    signal(SIGUSR2,transform_sucess);
    if (argc < 2 && strcmp(argv[1], "status") && strcmp(argv[1], "transform")) {
        perror_invalid_args();
        return -1;
    }
    // if non existent creates fifo client to server
    int fifo_ret = mkfifo(CLIENT_TO_SERVER, 0666);
    if (fifo_ret == -1 && errno != EEXIST) {
        perror(CLIENT_TO_SERVER);
    }
    // open write to server
        int fd_write = open(CLIENT_TO_SERVER, O_WRONLY, 0666);
        if(fd_write == -1) {
            perror(CLIENT_TO_SERVER);
            return -1;
        }

    if (!strcmp(argv[1], "status")) {
         // write command to server
        write(fd_write,argv[1],strlen(argv[1]));
        close(fd_write);
        //open fifo client to server 
        int fd_read = open(SERVER_TO_CLIENT,O_RDONLY,0666);
        char * buffer[STATUS_SIZE];
        ssize_t bytes;
        // read from server and write in stdout
        while((bytes = read(fd_read,buffer,STATUS_SIZE)) > 0)
            write(STDOUT_FILENO,buffer,bytes);
        close(fd_read);
    }
    else if(!strcmp(argv[1], "transform")) {
        char command[COMMAND_SIZE] = "";
        char pid[32] = "";
        sprintf(pid,"%d",getpid());
        strcat(command,pid);
        strcat(command," ");
        for(int i = 1; i < argc; i++){ 
            strcat(command,argv[i]);
            strcat(command," ");
        }
        // write command to server
        write(fd_write,command,strlen(command));
        close(fd_write);
        pause();

    }

    return 0;
}
//./ aurras transform in out tranf