#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

struct Job {
    int jobNumber;
    pid_t pid;
    char commandLine[512];
};
//function to return full path or null if not found(part 7)
char* get_full_path(char *cmd)
{
	char *path = getenv("PATH");
	if (!path) {
    fprintf(stderr, "PATH environment variable not set.\n");
    return NULL;
}
	char *pathCpy = strdup(path);
	char *dir = strtok(pathCpy, ":");

	while(dir != NULL)
	{
		char executable[512];
		snprintf(executable, sizeof(executable), "%s/%s", dir, cmd);
		if(access(executable, F_OK) == 0 && access(executable, X_OK) ==0)
		{
			free(pathCpy);
			return strdup(executable);  //return executable path
		}
		dir = strtok(NULL, ":");
	}
	free(pathCpy);
	return NULL; //command not found, return null
}
/*
//function to execute command in a child process with path(part 7)
void execute_cmd_with_path(char *cmd, int writePipe[2], int readPipe[2]) {
    if (fork() == 0) { //creates child process using fork
        if (writePipe) //If there's a write pipe, it duplicates its write end to standard output
		{
            dup2(writePipe[1], STDOUT_FILENO);
            close(writePipe[0]);
            close(writePipe[1]);
        }

        if (readPipe)  //If there's a read pipe, it duplicates its read end to standard input
		{
            dup2(readPipe[0], STDIN_FILENO);
            close(readPipe[0]);
            close(readPipe[1]);
        }
		//Executes the command using get_full_path
        char *fullPath = get_full_path(cmd);
        if (fullPath) {
            char *cmd_args[] = { cmd, NULL };
            execv(fullPath, cmd_args);
            free(fullPath);
        } else {
            perror("Command not found/not executable");
            exit(EXIT_FAILURE);
        }
    }
}*/

int main()
{
	bool error0 = false;
	//FOR PT9 INTERNAL COMMAND EXECUTION JOBS
	struct Job jobList[10];
	for(int i = 0; i < 10; i++)
		jobList[i].jobNumber = 0;
	int jobCount = 0;
	int jobsRunning = 0;

	//FOR PT9 INTERNAL COMMAND EXECUTION EXIT
	int commandHistory = 0;
	int totalCommandHistory = 0;
	char *cmd0 = (char *)malloc(sizeof(char *) * 200);
	char *cmd1 = (char *)malloc(sizeof(char *) * 200);
	char *cmd2 = (char *)malloc(sizeof(char *) * 200);
	char *tempcmd = (char *)malloc(sizeof(char *) * 200);
	while (1) {
		for(int i = 0; i < 10; i++)
		{
			pid_t pid = waitpid(jobList[i].pid, NULL, WNOHANG);
			if(pid > 0)
			{
				jobList[i].jobNumber = 0;
				jobsRunning--;
				printf("\n[Job %d] done\n", pid);
				printf("\n");
			}
		}

		size_t size = 0;
		size = pathconf(".", _PC_PATH_MAX);
		//ENVIRONMENTAL VARIABLES FOR PROMPT (PT1)
		const char *user = getenv("USER");
		const char *machine = getenv("MACHINE");
		char *pwd = NULL;		
		if ((pwd = (char *)malloc((size_t)size)) != NULL) 
        	getcwd(pwd, (size_t)size);

		//VARIABLES TO BE USED FOR ALL ERROR MESSAGES
		char *errorMessage;
		bool error = false;

		//PRINTS PROMPT
		printf("%s@%s:%s",user,machine,pwd);

		//PROVIDED CODE
		printf("> ");

		/* input contains the whole command
		 * tokens contains substrings from input split by spaces
		 */

		char *input = get_input();
		tokenlist *tokens = get_tokens(input);
		for (int i = 0; i < tokens->size; i++) {
		}

		//FOR PT9 EXIT (INTERNAL COMMANDS)
		free(tempcmd);
		tempcmd = (char *)calloc(200, sizeof(char));
		for(int i = 0; i < tokens->size; i++)
		{
			strncat(tempcmd, tokens->items[i], strlen(tokens->items[i]));
			strncat(tempcmd, " ", strlen(" "));
		}

		//ITERATE THROUGH TOKENS FOR ENVIRONMENT VARIABLE EXPANSION (PT2)
		for (int i = 0; i < tokens->size; i++)
		{
			//IF CURRENT TOKEN IS AN ENVIRONMENTAL VARIABLE
			if(tokens->items[i][0]=='$')
			{				
				char variable[50];
				printf("%s", tokens->items[i]);
				strncpy(variable, tokens->items[i] + 1, strlen(tokens->items[i]));
				variable[strlen(variable)] = '\0';

				const char *envvar = getenv(variable);

				//ERROR CHECKING FOR BAD ENVIRONMENTAL VARIABLE
				if(envvar == NULL)
				{
					errorMessage = "ERROR: BAD ENVIRONMENTAL VARIABLE";
					error = true;
				}
				else
				{
					//IF ENVIRONMENTAL VARIABLE EXISTS, REALLOCATE TOKEN TO VARIABLE OF $___
					tokens->items[i] = (char *)realloc(tokens->items[i], strlen(envvar) + 1);
					strcpy(tokens->items[i], envvar);
				}
			}

			//TILDE EXPANSION
			//Checking if the token starts with a tilde
			if(tokens->items[i][0] == '~' && (tokens->items[i][1] == '\0' || tokens->items[i][1] == '/'))
			{
				//Get HOME evironment variable
				char *home = getenv("HOME");

				if(home)  //If $HOME is set
				{
					//Assign space for the expanded path
					//using malloc() to allocate the requested memory and return pointer to it
					char *expandedPath = (char *)malloc(strlen(home) + strlen(tokens->items[i]) + 1);
					//creating expanded path
					strcpy(expandedPath, home);
					strcat(expandedPath, tokens->items[i] + 1);

					//using free() to deallocates tokens previous memory
					free(tokens->items[i]);
					//Updating tokens with expanded path
					tokens->items[i] = expandedPath;
				}
				else  //Show error message if $HOME is not set
				{
					errorMessage = "ERROR: Tilde expansion failed; HOME environment variable is not set.\n";
					error = true;
				}
			}

			//PATH SEARCH (PT4)
			//get the whole $PATH variable
			char * ptrPath = getenv("PATH");
			char path[256];
			strcpy(path, ptrPath);


			//delimit path variable by : (seperate directories)
			char* token = strtok(path, ":");
			//bool isExecutable = false;
			//check if the token is executable (it is already a path)
			/*bool wantToExpand = true;
			if(strcmp(tokens->items[i], "cd") == 0 || strcmp(tokens->items[i], "exit") == 0
			|| strcmp(tokens->items[i], "jobs") == 0 || (i != 0 
			&& strcmp(tokens->items[i-1],"|") != 0 && strcmp(tokens->items[i-1],"<") != 0
			&& strcmp(tokens->items[i-1],">") != 0))
				wantToExpand = false;//to prevent unwanted variables from being expanded
			if(tokens->items[i+1] != NULL && strcmp(tokens->items[i+1], "<") == 0)
				wantToExpand = false;
			if(i > 0 && strcmp(tokens->items[i-1], ">") == 0)
				wantToExpand = false;*/

			bool wantToExpand = true;//PREVENT EXPANSION OF FILE NAMES (I/O), INTERNAL COMMANDS, AND FLAGS
			if(strcmp(tokens->items[i], "cd") == 0)
				wantToExpand = false;
			else if(strcmp(tokens->items[i], "exit") == 0)
				wantToExpand = false;
			else if(strcmp(tokens->items[i], "jobs") == 0)
				wantToExpand = false;
			else if(tokens->items[i+1] != NULL && strcmp(tokens->items[i+1], "<") == 0)
				wantToExpand = false;
			else if(i > 0 && strcmp(tokens->items[i-1], ">") == 0)
				wantToExpand = false;
			else if(tokens->items[i+1] != NULL && strcmp(tokens->items[i+1], ">") == 0)
				wantToExpand = true;
			else if(i > 0 && strcmp(tokens->items[i-1], "<") == 0)
				wantToExpand = true;
			else if(i != 0 && strcmp(tokens->items[i-1],"|") != 0)
				wantToExpand = false;
				
			if((access(tokens->items[i], F_OK) != 0 || access(tokens->items[i], X_OK) != 0)
			&& wantToExpand == true)
			{
				//iterates through $PATH directories seperated by :
				while(token != NULL)
				{
					//concatenate each $PATH directory with the command/token
					char executable[256];
    				strcpy(executable, token); // Copy the first part
    				strcat(executable, "/"); // Concatenate a forward slash
    				strcat(executable, tokens->items[i]);
					//if it is executable, copy to finalExecutable
					if(access(executable, F_OK) == 0 && access(executable, X_OK) == 0)
					{
						tokens->items[i] = (char *)realloc(tokens->items[i], strlen(executable) + 1);
                        strcpy(tokens->items[i], executable);
                        strcpy(tokens->items[i], executable);
                        //printf("~~New token:%s\n",tokens->items[i]);
						strcpy(tokens->items[i], executable);
                        //printf("~~New token:%s\n",tokens->items[i]);
						//isExecutable = true;
						break;	
					}
					token = strtok(NULL, ":");
				}
				//if it is executable, copy final executable into token item
				//if(isExecutable)
				//{
					//tokens->items[i] = (char *)realloc(tokens->items[i], strlen(finalExecutable) + 1);
					//strcpy(tokens->items[i], finalExecutable);
				//}
				//else
				//{
				//	if()
				//	errorMessage = "ERROR: Command not found or not executable.\n";
				//	error = true;
				//}
			}
		}

		/*
		//PART 7 PIPING
		int pipe1[2];
		int pipe2[2];
		//checking how many pipes are present
		int pipeCount = 0;
		int index1 = 0, index2 = 0;
		for(int i = 0; i < tokens->size; i++)
		{
			if(strcmp(tokens->items[i], "|") == 0)
			{
				pipeCount++;
				if(pipeCount == 1)
					index1 = i;
				if(pipeCount == 2)
					index2 = i;
			}
		}
		//Handling execution commands based on number of pipes in command line
		switch (pipeCount)
		{
		case 1: //case one for guideline cmd1 | cmd2 (cmd1 redirects its standard output to the standard input of cmd2)
			if (pipe(pipe1) == -1) 
			{
            	perror("pipe1 failed");
            	exit(EXIT_FAILURE);
        	}
			//Uses the execute_cmd_with_path function to execute the two commands with appropriate read and write pipes
			execute_cmd_with_path(tokens->items[0], pipe1, NULL);
        	execute_cmd_with_path(tokens->items[index1 + 1], NULL, pipe1);
			close(pipe1[0]);
			close(pipe1[1]);
			//wait for child processes to finish execution
			wait(NULL);  
        	wait(NULL);	
			break;

		case 2:  //case one for guideline cmd1 | cmd2 | cmd3
			if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
			{
            	perror("pipe failed");
            	exit(EXIT_FAILURE);
        	}
			execute_cmd_with_path(tokens->items[0], pipe1, NULL);
			execute_cmd_with_path(tokens->items[index1 + 1], pipe2, pipe1);
			execute_cmd_with_path(tokens->items[index2 + 1], NULL, pipe2);
			//in parent process, close both ends of pipe1 and pipe2
			close(pipe1[0]);
			close(pipe1[1]);
			close(pipe2[0]);
			close(pipe2[1]);
			//make parent process wait for all three child processes to finish execution
			wait(NULL);
			wait(NULL);
			wait(NULL);
			break;
		default:
			fprintf(stderr, "Unsupported number of pipes: %d\n", pipeCount);
			break;
		} //end of part 7 */

		if(error)
		{
			//DISPLAY ANY ERROR MESSAGE HERE THAT HAPPENS BEFORE COMMAND EXECUTION
			printf("%s\n",errorMessage);
		}
		else
		{
			
			int IOorPipe = false;//SO THAT #5, 8, & 9 DO NOT INTERFERE WITH 6 & 7
			for(int i = 0; i < tokens->size; i++)
			{
				if(strcmp(tokens->items[i], "|") == 0 || strcmp(tokens->items[i], ">") == 0
				|| strcmp(tokens->items[i], "<") == 0)
					IOorPipe = true;
			}

			//BACKGROUND PROCESSING (TEMPORARILY EXCLUDES IO AND PIPE)
			if(tokens->size != 0 && tokens->items[tokens->size -1][0] == '&' && IOorPipe == false)
			{
				int status;
				pid_t pid = fork();
				if(pid == 0) {
					tokens->items[tokens->size -1] = NULL;
					if (access(tokens->items[0], X_OK) == 0)
					{
						execv(tokens->items[0], tokens->items);
					}
					else
					{
						printf("ERROR: Command not found or not executable.\n");
						error = true;
					}
				}
				else {
					jobCount++;
					jobsRunning++;
					printf("[%d] [%d]\n", jobCount, getpid());
					if(!error && (jobsRunning <= 10))
					{
						for(int i = 0; i < 10; i++)
						{
							if(jobList[i].jobNumber == 0)
							{
								jobList[i].jobNumber = jobCount;
    							jobList[i].pid = getpid();
								strncpy(jobList[i].commandLine, tempcmd, sizeof(jobList[i].commandLine));
								break;
							}
						}
					}
					else if(jobsRunning > 10)
					{
						error = true;
						printf("ERROR: maximum number of background jobs to be displayed reached\n");
					}
					waitpid(pid, &status, WNOHANG);
				}
			}
			else if(IOorPipe == false)//PART 9 (INTERNAL COMMAND EXECUTION)
			{
				if(strcmp(tokens->items[0], "exit") == 0)
				{
					//waitpid(pid, &status, WNOHANG);
					if(totalCommandHistory == 3)
					{
						printf("Last (%d) valid commands:\n", totalCommandHistory);
						printf("[1]: %s\n", cmd0);
						printf("[2]: %s\n", cmd1);
						printf("[3]: %s\n", cmd2);
					}
					else if(totalCommandHistory > 0)
					{
						printf("Last valid command:\n");
						if(totalCommandHistory%3 == 1)
							printf("[1]: %s\n", cmd0);
						else if(totalCommandHistory%3 == 2)
							printf("[1]: %s\n", cmd1);
						else if(totalCommandHistory%3 == 3)
							printf("[1]: %s\n", cmd2);
					}
					else
					{
						printf("No valid commands in history.\n");
					}
					exit(0);
				}
				else if(strcmp(tokens->items[0], "cd") == 0)
				{
					if(tokens->size > 2 && strcmp(tokens->items[2], "|") != 0 && 
					strcmp(tokens->items[2], "<") != 0 && strcmp(tokens->items[2], ">") != 0)
					{
						error = true;
						printf("ERROR: too many arguments\n");
					}
					else if(tokens->items[1] == NULL || strlen(tokens->items[1]) < 0)
					{
						chdir(getenv("HOME"));
					}
					else if(chdir(tokens->items[1]) != 0)
					{
						if(access(tokens->items[1], F_OK) != 0)
    						printf("ERROR: directory does not exist\n");
						else
							printf("ERROR: not a directory\n");
						error = true;
						printf("Token at index 1 (directory path): %s\n", tokens->items[1]);
					}
				}
				else if(strcmp(tokens->items[0], "jobs") == 0)
				{
					for (int i = 1; i <= 10; i++) 
					{
						if(jobList[i].jobNumber > 0)
							printf("[%d]+ %d %s\n", jobList[i].jobNumber, jobList[i].pid, jobList[i].commandLine);
					}
				}
				else if(access(tokens->items[0], F_OK) == 0 && access(tokens->items[0], X_OK) == 0)//PART 5 (NO PIPE OR IO)
				{
					int status;
					pid_t pid = fork();
					if (pid == 0)
					{
						execv(tokens->items[0], tokens->items);
					}
					else {
						waitpid(pid, &status, 0);
						//exit(0);
					}
				}
				else
				{
					error = true;
					printf("ERROR: Command not found or not executable.\n");
				}
				
			}
		}

		//COPY COMMAND FOR PT9 INTERNAL COMMAND EXECUTION: EXIT
		if(!error)//MAKE SURE TO SET ERROR TO 0 IF COMMAND NOT WORK
		{
			if(commandHistory == 0)
			{
				free(cmd0);
				cmd0 = (char *)calloc(200, sizeof(char));
				strncat(cmd0, tempcmd, strlen(tempcmd));
				commandHistory++;
				totalCommandHistory++;
			}
			else if(commandHistory == 1)
			{
				free(cmd1);
				cmd1 = (char *)calloc(200, sizeof(char));
				strncat(cmd1, tempcmd, strlen(tempcmd));
				commandHistory++;
				totalCommandHistory++;
			}
			else if(commandHistory == 2)
			{
				free(cmd2);
				cmd2 = (char *)calloc(200, sizeof(char));
				strncat(cmd2, tempcmd, strlen(tempcmd));
				commandHistory = 0;
				totalCommandHistory++;
			}
		}
		error0 = error;
		free(input);
		free_tokens(tokens);
		//FREE HERE
		free(pwd);
	}

	//COPY COMMAND FOR PT9 INTERNAL COMMAND EXECUTION: EXIT
	if(!error0)//MAKE SURE TO SET ERROR TO 0 IF COMMAND NOT WORK
	{
		if(commandHistory == 0)
		{
			free(cmd0);
			cmd0 = (char *)calloc(200, sizeof(char));
			strncat(cmd0, tempcmd, strlen(tempcmd));
			commandHistory++;
			totalCommandHistory++;
		}
		else if(commandHistory == 1)
		{
			free(cmd1);
			cmd1 = (char *)calloc(200, sizeof(char));
			strncat(cmd1, tempcmd, strlen(tempcmd));
			commandHistory++;
			totalCommandHistory++;
		}
		else if(commandHistory == 2)
		{
			free(cmd2);
			cmd2 = (char *)calloc(200, sizeof(char));
			strncat(cmd2, tempcmd, strlen(tempcmd));
			commandHistory = 0;
			totalCommandHistory++;
		}
	}

	return 0;
}

char *get_input(void) {
	char *buffer = NULL;
	int bufsize = 0;
	char line[5];
	while (fgets(line, 5, stdin) != NULL)
	{
		int addby = 0;
		char *newln = strchr(line, '\n');
		if (newln != NULL)
			addby = newln - line;
		else
			addby = 5 - 1;
		buffer = (char *)realloc(buffer, bufsize + addby);
		memcpy(&buffer[bufsize], line, addby);
		bufsize += addby;
		if (newln != NULL)
			break;
	}
	buffer = (char *)realloc(buffer, bufsize + 1);
	buffer[bufsize] = 0;
	return buffer;
}

tokenlist *new_tokenlist(void) {
	tokenlist *tokens = (tokenlist *)malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->items = (char **)malloc(sizeof(char *));
	tokens->items[0] = NULL; /* make NULL terminated */
	return tokens;
}

void add_token(tokenlist *tokens, char *item) {
	int i = tokens->size;

	tokens->items = (char **)realloc(tokens->items, (i + 2) * sizeof(char *));
	tokens->items[i] = (char *)malloc(strlen(item) + 1);
	tokens->items[i + 1] = NULL;
	strcpy(tokens->items[i], item);

	tokens->size += 1;
}

tokenlist *get_tokens(char *input) {
	char *buf = (char *)malloc(strlen(input) + 1);
	strcpy(buf, input);
	tokenlist *tokens = new_tokenlist();
	char *tok = strtok(buf, " ");
	while (tok != NULL)
	{
		add_token(tokens, tok);
		tok = strtok(NULL, " ");
	}
	free(buf);
	return tokens;
}

void free_tokens(tokenlist *tokens) {
	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);
	free(tokens->items);
	free(tokens);
}