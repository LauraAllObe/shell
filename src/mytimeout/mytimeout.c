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
//grab path
char* get_path(char *cmd)
{
	char *path = getenv("PATH");
	if(!path)
	{
		fprintf(stderr, "PATH environment variable not set.\n");
		return NULL;
	}
	
	char *pathCopy = strdup(path);
	char *dir = strtok(pathCopy, ":");

	while (dir!=NULL)
	{
		char executable[512];
		snprintf(executable, sizeof(executable), "%s/%s", dir, cmd);
		if (access(executable, F_OK == 0 && access(executable, X_OK) == 0))
		{
			free(pathCopy);
			return strdup(executable);
		}
		dir = strtok(NULL, ":");
	}

	free(pathCopy);
	return NULL;
}
/*signal handler for the alarm signal. This handler will be triggered when a process receives an SIGALRM
 signal, which will be sent when the timer we'll set (with alarm()) expires.*/
void alarm_handler(int signum) {
     if (child_pid != 0) {
        kill(child_pid, SIGTERM);
    }
}

int main(int argc, char *argv[]){
//printf("%d", argc);
    if (argc < 3) {
        printf("Usage: ./mytimeout [snds] [cmd] [cmd-args].\n");
        exit(1);
    }
 //printf("%d", argc);
    int duration = atoi(argv[1]);

    char **exec_argv = (char **)malloc((argc-2) * sizeof(char *));	//get arguments, starts grab at argv[2]

    if (exec_argv == NULL)
    {
	perror("Memory allocation failed");
	exit(1);
    }

    for (int i = 0; i < argc-2; i++)
    {
    	exec_argv[i] = NULL;
    }
    for (int i = 2; i < argc; i++)
    {
	exec_argv[i-2] = argv[i];
    }
    exec_argv[argc-2] = NULL;
	/*for(int i = 0; i < argc-2; i++)
		{
			printf("%s", exec_argv[i]);
		}
printf("\n");
    char *cmd = exec_argv[0];	//OLD PATH GRAB, BARELY WORKS
    char pathstr[1024];
    int pathCount = 0;
    char paths[100][256];
    FILE *path = popen(cmd, "r");
    if (path == NULL) 
    {
        perror("popen");
        return 1;
    }
    
    while (fgets(pathstr, sizeof(pathstr), path) != NULL) 
    {
        // Remove trailing newline
        if (pathstr[strlen(pathstr) - 1] == '\n') {
            pathstr[strlen(pathstr) - 1] = '\0';
        }
	strncpy(paths[pathCount], pathstr, sizeof(paths[pathCount]));
        pathCount++;

        // Check if we've reached the maximum number of paths
        if (pathCount >= 100) {
            printf("Maximum number of paths reached.\n");
            break;
        }
    }*/

    char* fullPath = get_path(argv[2]);
    if(!fullPath)
    {
	perror("Command not found/not executable");
    }
	//printf("%s", fullPath);
    pid_t pid = fork();

    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if(pid == 0){
        //execute command
	/*for (int i = 0; i < pathCount; i++) {		//halfway works w/ OLD PATH GRAB
        char *exec_argv[] = {paths[i], NULL};
        if (execv(paths[i], exec_argv) == -1) {
            perror("execv");
        }*/
       execv(fullPath, exec_argv);
    
        
    }else{
        //part 10. this allows to capture the SIGALRM signal whenever an alarm goes off
        signal(SIGALRM, alarm_handler);
        alarm(duration);

        waitpid(pid, 0, 0);
        
    }
    

   free(exec_argv);
    
    return 0;

}