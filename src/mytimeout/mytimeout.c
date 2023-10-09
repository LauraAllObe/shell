#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//global variable for child process pt10
pid_t child_pid = 0;
//part 10

/*signal handler for the alarm signal. This handler will be triggered when a process receives an SIGALRM
 signal, which will be sent when the timer we'll set (with alarm()) expires.*/
void alarm_handler(int signum) {
     if (child_pid != 0) {
        kill(child_pid, SIGTERM);
    }
}

int main(int argc, char *argv[]){
    if (argc < 3) //error when too few arguments
    {
        printf("Usage: ./mytimeout [snds] [cmd] [cmd-args].\n");
        return 0;
    }
    int duration = atoi(argv[1]);

    char *exec_argv[argc-1];//ALLOCATE SPACES IN EXEC_ARGV TO BE COMMAND ARGUMENTS
    for (int i = 2; i < argc; i++)
	    exec_argv[i-2] = argv[i];
    exec_argv[argc-2] = NULL;
    
    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        return 0;
    }
    if(pid == 0){
       execv(exec_argv[0], exec_argv);
    }else{
        //part 10. this allows to capture the SIGALRM signal whenever an alarm goes off
        signal(SIGALRM, alarm_handler);
        alarm(duration);

        waitpid(pid, 0, 0);
    }    
    return 0;
}