#include "utilities.h"
char command[COMMAND_SIZE] = "";
pid_t pid;

void send_used_command() {
    char usedcommand[COMMAND_SIZE] = "used ";
    strcat(usedcommand,command);
    int fd_write = open(CLIENT_TO_SERVER, O_WRONLY, 0666);
    if(fd_write == -1) {
        perror(CLIENT_TO_SERVER);
        return;
    }
    strcat(usedcommand,"\n");
    write(fd_write,usedcommand,strlen(usedcommand));
    close(fd_write);
}
void tranform_error(int signal) {
    write(1,"Transform failed!\n",strlen("transform failed\n"));
    send_used_command();
    kill(pid,SIGKILL);
}
void transform_sucess(int signal) {
    write(1,"Done!\n",strlen("done\n"));
    send_used_command();
    kill(pid,SIGKILL);
}
void h_processing(int signal) {
    write(1,"Processing...\n",strlen("Processing...\n"));
    pause();
}

int main(int argc, char **argv) {
    //
    pid = getpid();
    signal(SIGUSR1,tranform_error);
    signal(SIGUSR2,transform_sucess);
    signal(SIGPOLL,h_processing);
    if (argc < 2 && strcmp(argv[1], "status") && strcmp(argv[1], "transform")) {
        write(STDOUT_FILENO,"Invalid args!\n",strlen("Invalid args!\n"));
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
        strcat(command,argv[1]);
        strcat(command," \n");
        write(fd_write,command,strlen(argv[1]));
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
        char pidstr[32] = "";
        sprintf(pidstr,"%d",pid);
        strcat(command,pidstr);
        for(int i = 1; i < argc; i++){ 
            strcat(command," ");
            strcat(command,argv[i]);
        }
        strcat(command," \n");
        // write command to server
        write(fd_write,command,strlen(command));
        close(fd_write);
        write(1,"Pending...\n",strlen("Pending...\n"));
        pause();

    }

    return 0;
}
//./ aurras transform in out tranf