#include "filters.h"
#include "utilities.h"
#include <ctype.h>
FILTERS fs;
char *pending[16];
int n_pending;
int n_processing;
char *processing[16];
char** divide_command(char* command) {
    if(command == NULL) return NULL;
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
        if (!(arg && *arg )) continue;  // in case of empty string
        params[i] = strdup(arg);
    }
    return params;
}
int exec_status() {
    int fd = open(SERVER_TO_CLIENT, O_WRONLY, 0777);
    if (fd == -1 && errno != EEXIST) {
        perror(SERVER_TO_CLIENT);
        return -1;
    }
    char* out = status(fs);
    write(fd, out, strlen(out));
    for(int i = 0; i< n_processing; i++) write(fd,processing[i], strlen(processing[i]));
    close(fd);
    return 0;
}

int exec_transform(char** args) {
    char* audio_in = args[2];   // sample name
    char* audio_out = args[3];  // sample result name
    //open sample
    int f_in = open(audio_in, O_RDONLY, 0666);
    if (f_in == -1) {
        return -1;
    }
    // open result
    int f_out = open(audio_out, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (f_in == -1) {
        return -1;
    }
    dup2(f_in,0);
    dup2(f_out,1);
    int fd[2];
    int i = 4;
    for (; i < MAX_ARGS && args[i+1]; i++) {
        if (pipe(fd) == -1) {
            return -1;
        }
        filter f = find_filter(args[i],fs);
        int pid_child = fork();
        if (pid_child == 0) {
            dup2(fd[1], 1);
            close(fd[1]);
            close(fd[0]);
            if (execl(f->path, f->path, NULL)) {
               _exit(-1);
            }

        } else {
            dup2(fd[0], 0);
            close(fd[0]);
            close(fd[1]);
        }
    }
    pid_t pid;
    filter f = find_filter(args[i],fs);
    if ((pid = fork()) == 0) {
        if (execl(f->path, f->path, NULL)) {
            _exit(-1);
        }
    }
    else {
        int status;
        waitpid(pid,&status,0);
        return WEXITSTATUS(status);
    }
    return 0;
}
int exec_command(char* command) {
    if(command == NULL) return -2;
    char** args = divide_command(strdup(command));
    if (!args) return -2;
    if (!strcmp(command, "status")) {
        exec_status();
    } 
    else if(*args && !strcmp(args[0], "used")) {
        for(int i = 5; args[i] ;i++) free_filter(fs,args[i]);
        int i;
        int j;
        for(j = 0; j <= n_processing; j++) if(strstr(command,args[1])) break;
        for(int a = j; a <= n_processing; a++) processing[a] = processing[a+1];
        n_processing--;
        do { 
            for(i = 0; i < n_pending; i++) 
                if(can_transform(fs,divide_command(pending[n_pending]+4))) break;
            
            if(i < n_pending) { 
                for(int j = i; j <= n_pending; j++) pending[i] = pending[i+1];
                n_pending--;
            }
            exec_command(pending[n_pending]);
        }while(i < n_pending);
        return 0;
    }
    else if (*args && !strcmp(args[1], "transform")) { 
        //ocupy filters
        if(!can_transform(fs,args+4)){
            pending[n_pending++] = strdup(command); 
            printf("PENDING %s\n", command);
            return -1;
        }
        kill(atoi(args[0]),SIGPOLL);
        char buf[COMMAND_SIZE] = "";
        sprintf(buf,"task#%d %s\n",n_processing+1 ,command);

        processing[n_processing++] = strdup(buf);
        for(int i = 4; args[i]; i++) ocup_filter(fs,args[i]);
        
        if(fork() == 0) {
            pid_t pid;
            if((pid = fork()) == 0) { 
                _exit(exec_transform(args));
            }
            
            int status;
            waitpid(pid,&status,0);
            if(WEXITSTATUS(status)) 
                kill(atoi(args[0]),SIGUSR1);
            else kill(atoi(args[0]),SIGUSR2);
            _exit(0);
        }
    }
    return 0;
}
int main(int argc, char** argv) {
    if (argc < 3) perror_invalid_args();

    fs = fill_filters(argv[1]);
    if (fs == NULL) return -1;
    n_pending = 0;
    n_processing = 0;
    for(int i = 0; i < 16; i++) {pending[i] = NULL; processing[i] = NULL;}
    int fifo_ret = mkfifo(SERVER_TO_CLIENT, 0666);
    if (fifo_ret == -1 && errno != EEXIST) {
        perror(SERVER_TO_CLIENT);
        return -1;
    }
    while (1) {
        // open fifo to read from client
        int fd = open(CLIENT_TO_SERVER, O_RDONLY, 0666);
        char *command = malloc(COMMAND_SIZE);
        ssize_t bytes = read(fd, command, COMMAND_SIZE);
        if (bytes > 0){ 
         char * buf = strdup(strsep(&command,"\n"));
         exec_command(buf);  
         }            
    }
    return 0;
}
