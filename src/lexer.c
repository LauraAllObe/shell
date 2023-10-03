#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

struct Job {
    int jobNumber;
    pid_t pid;
    char commandLine[512];
};

//global pipe file descriptors (part7)
int pipe1[2];
int pipe2[2];

//function to execute a command (part 7)
void execute_command(char *cmd, char **args)
{
	if(execvp(cmd, args) == -1)
	{
		perror("execvp");
		exit(EXIT_FAILURE);
	}
}

//function to return full path or null if not found(part 7)
char* get_full_path(char *cmd)
{
	char *path = getenv("PATH");
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

int main()
{
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
				printf(tokens->items[i]);
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
			if(tokens->items[i][1] == '~')
			{
				//If tilde stands alone or tilde is followed by a '/'
				if(tokens->items[i][1] == '\0' || tokens->items[i][1] == '/')
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
//PT6
		if((strcmp(tokens->items[0], "cmd")==0)||(strcmp(tokens->items[0], "CMD")==0))
			{
				//FILE OUT
				if(strcmp(tokens->items[1], ">")==0)
				{
					if(tokens->size > 3)
					{		//MULTISTEP
						if(strcmp(tokens->items[3], "<")==0)
						{
							int rediri = open(tokens->items[4], O_RDONLY);
							if(rediri == -1)
							{
								perror("The file requested does not exist or is not a regular file.");
							} else
							{
								dup2(rediri, STDIN_FILENO);
								close(rediri);
							}
						}
					}
					int rediro = open(tokens->items[2], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); //important note: logical or (||) will cause the program to panic. Use (|) for flag seperation.
					dup2(rediro, STDOUT_FILENO);
					close(rediro);
				} //FILE IN
				else if(strcmp(tokens->items[1], "<")==0)
				{
					int rediri = open(tokens->items[2], O_RDONLY);
					if(rediri == -1)
					{
						perror("The file requested does not exist or is not a regular file.");
					} else
					{
						dup2(rediri, STDIN_FILENO);
						close(rediri);
					}
					if(tokens->size > 3)
					{		//MULTISTEP
						if(strcmp(tokens->items[3], ">")==0)
						{
							int rediro = open(tokens->items[4], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
							dup2(rediro, STDOUT_FILENO);
							close(rediro);
						}
					}
				}

			}
			
		}


		//PART 7 PIPING
		//checking how many pipes are present
		/*int pipeCount = 0;
		int index1, index2 = 0, 0;
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
			pipe(pipe1);
			if(fork() == 0) //creates child process using fork, 1st child for command 1
			{
				dup2(pipe1[1], STDOUT_FILENO);//whatever the child process write to its standard output will be writen into pipe1
				//close the read-end and also the write-end of pipe1 in the child process
				close(pipe1[0]);
				close(pipe1[1]);
				char *fullPath = get_full_path(tokens->items[0]);
				if(fullPath)
				{
					execv(fullPath, &tokens->items[0]);
					free(fullPath);
				} else
				{
					perror("Command not found/not executable");
					exit(EXIT_FAILURE);//if execv fails, the child process terminates with a failure status
				}
			}

			if(fork() == 0)  //creates a new child process, 2nd child for command 2
			{
				//using dup2 to redirect STDIN(anything the child process reads from its standard input will now be read from pipe1)
				dup2(pipe1[0], STDIN_FILENO);  
				close(pipe1[0]);
				close(pipe1[1]);  //closes the write-end of pipe1
				char *fullPath = get_full_path(tokens->items[index1 + 1]);
				if(fullPath)
				{
					execv(fullPath, &tokens->items[0]);
					free(fullPath);
				} else
				{
					perror("Command not found/not executable");
					exit(EXIT_FAILURE);//if execv fails, the child process terminates with a failure status
				}
			}
			//close both ends of pipe1 in the parent process
			close(pipe1[0]); 
			close(pipe1[1]);
			//make parent process wait for both child processes to complete execution
			wait(NULL);  
			wait(NULL);
			break;

		case 2:  //case one for guideline cmd1 | cmd2 | cmd3
			pipe(pipe1);
			pipe(pipe2);
			if(fork() == 0)  //create child process, 1st child for command 1
			{
				//Redirects the child's standard output to the write-end of pipe1
				dup2(pipe1[1], STDOUT_FILENO);
				//close both ends of pipe1 and pipe2
				close(pipe1[0]);
				close(pipe1[1]);
				close(pipe2[0]);	
				close(pipe2[1]);	
				char *fullPath = get_full_path(tokens->items[0]);
				if(fullPath)
				{
					execv(fullPath, &tokens->items[0]);
					free(fullPath);
				} else
				{
					perror("Command not found/not executable");
					exit(EXIT_FAILURE);//if execv fails, the child process terminates with a failure status
				}
			}

			if(fork() == 0)  //2nd child for command 2
			{
				dup2(pipe1[0], STDIN_FILENO); //redirect the child's standard input to the read-end of pipe1
				dup2(pipe2[1], STDOUT_FILENO); //redirects the child's standard output to the write-end of pipe2
				//close both ends of pipe1 and pipe2
				close(pipe1[0]);
				close(pipe1[1]);
				close(pipe2[0]); 
				close(pipe2[1]); 
				char *fullPath = get_full_path(tokens->items[index1 + 1]);
				if(fullPath)
				{
					execv(fullPath, &tokens->items[0]);
					free(fullPath);
				} else
				{
					perror("Command not found/not executable");
					exit(EXIT_FAILURE);//if execv fails, the child process terminates with a failure status
				}
			}

			if(fork() == 0)  //3rd child for command 3
			{
				dup2(pipe2[0], STDIN_FILENO); //redirects the child's standard input to the read-end of pipe2
				//close both ends of pipe1 and pipe2
				close(pipe1[0]);
				close(pipe1[1]);
				close(pipe2[0]);
				close(pipe2[1]);
				char *fullPath = get_full_path(tokens->items[index2 + 1]);
				if(fullPath)
				{
					execv(fullPath, &tokens->items[0]);
					free(fullPath);
				} else
				{
					perror("Command not found/not executable");
					exit(EXIT_FAILURE);//if execv fails, the child process terminates with a failure status
				}
			}
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

		free(input);
		free_tokens(tokens);

		//FREE HERE
		free(pwd);
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
