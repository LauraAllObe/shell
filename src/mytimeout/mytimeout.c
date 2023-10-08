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
    if (argc < 3) {
        printf("Usage: ./mytimeout [snds] [cmd] [cmd-args].\n");
        exit(1);
    }
    
    int duration = atoi(argv[1]);

    char **exec_argv = (char **)malloc((argc+1) * sizeof(char *));	//get arguments

    if (exec_argv == NULL)
    {
	perror("Memory allocation failed");
	exit(1);
    }

    for (int i = 0; i < argc; i++)
    {
	exec_argv[i] = argv[i];
    }

    exec_argv[argc] = NULL;

    pid_t pid = fork();

    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if(pid == 0){
        //execute command

        execv(exec_argv[2], exec_argv);
        //if execv fails
        perror("exec");
        exit(1);
        
    }else{
        //part 10. this allows to capture the SIGALRM signal whenever an alarm goes off
        signal(SIGALRM, alarm_handler);
        alarm(duration);

        waitpid(pid, 0, 0);
        
    }
    

   free(exec_argv);
    
    return 0;

}
