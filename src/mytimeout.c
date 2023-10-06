#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

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

    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if(pid == 0){
        //search path
        //execute command

        execvp(argv[2], &argv[2]);
        //if execv fails
        perror("exec");
        exit(1);
        
    }else{
        //part 10. this allows to capture the SIGALRM signal whenever an alarm goes off
        signal(SIGALRM, alarm_handler);
        alarm(duration);

        waitpid(pid, 0, 0);
        
    }
    

   
    
    return 0;

}

