#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~OFFICE HOUR QUESTIONS (TO KEEP TRACK):~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WHEN CD, DOES DIRECTORY GO IN PROMPT (ABSOLUTE WORKING DIRECTORY -> CURRENT WORKING DIRECTORY)?
CAN OUR PROJECT STRUCTURE FEATURE A .GITIGNORE FILE?
CAN WE HAVE LEXER.C AND LEXER.H INSTEAD OF SHELL.H, SHELL.C, AND MAIN.C?
CHECK PROJECT STRUCTURE WITH TA


*/
//JOB STRUCTURE FOR jOBS INTERNAL COMMAND EXECUTION (PART 9)
struct Job {
    int jobNumber;
    pid_t pid;
    char commandLine[512];
};

/*
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
}*/
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
	//FOR PART 9 INTERNAL COMMAND EXECUTION JOBS, INITIALIZE JOB LIST JOB # TO 0
	struct Job jobList[10];
	for(int i = 0; i < 10; i++)
		jobList[i].jobNumber = 0;
	int jobCount = 0;
	int jobsRunning = 0;

	//FOR PART 9 INTERNAL COMMAND EXECUTION EXIT, INITIALIZE PLACEHOLDERS AND 
	int commandHistory = 0;//COUNTERS FOR THE THREE MOST RECENT COMMANDS
	int totalCommandHistory = 0;
	char *cmd0 = (char *)malloc(sizeof(char *) * 200);
	char *cmd1 = (char *)malloc(sizeof(char *) * 200);
	char *cmd2 = (char *)malloc(sizeof(char *) * 200);
	char *tempcmd = (char *)malloc(sizeof(char *) * 200);

	//PT6 RETAIN I/O & BOOL FLAG FOR IF DRAWING/SENDING FROM/TO FILE
	int infd = dup(STDIN_FILENO);
	int outfd = dup(STDOUT_FILENO);
	bool isFileIn = 0;
	bool isFileOut = 0;
	int rediri = 0;//REDIRECT INPUT
	int rediro = 0; //REDIRECT OUTPUT- SAVE VARIABLE FOR LATER USE

	while (1) {

		if((isFileIn == 1) && (feof(stdin)))	//PT 6, SWITCH BACK CONTROL @ EOF
		{
			dup2(infd, STDIN_FILENO);			
			close(rediri);
			isFileIn = 0;
		}

		//DECREMENT BACKGROUND PROCCESSES WHEN COMPLETED FOR BACKGROUND PROCESSING (PART 8) AND
		for(int i = 0; i < 10; i++)//JOBS INTERNAL COMMAND EXECUTION (PART 9)
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
		//ENVIRONMENTAL VARIABLES FOR PROMPT (PART 1)
		const char *user = getenv("USER");
		const char *machine = getenv("MACHINE");
		char *pwd = NULL;		
		if ((pwd = (char *)malloc((size_t)size)) != NULL) 
        	getcwd(pwd, (size_t)size);

		//VARIABLES TO BE USED FOR MOST ERROR MESSAGES
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

		//FOR EXIT INTERNAL COMMAND EXECUTION (PART 9)
		//STORE THE COMMAND (WITHOUT PATH EXPANSION, ETC) FOR LATER USE
		free(tempcmd);
		tempcmd = (char *)calloc(200, sizeof(char));
		for(int i = 0; i < tokens->size; i++)
		{
			strncat(tempcmd, tokens->items[i], strlen(tokens->items[i]));
			strncat(tempcmd, " ", strlen(" "));
		}

		//ITERATE THROUGH TOKENS FOR ENVIRONMENT VARIABLE EXPANSION (PART 2)
		for (int i = 0; i < tokens->size; i++)
		{
			//EXTRA CREDIT #3 (EXECUTE SHELL WITHIN SHELL), THIS EXPANDS BIN/SHELL AND
			if(strcmp(tokens->items[i], "./bin/shell") == 0 //./BIN/SHELL TO FULL PATH
			|| strcmp(tokens->items[i], "bin/shell") == 0 )
			{
				tokens->items[0] = (char *)realloc(tokens->items[0], strlen(pwd) + 1);
				strncpy(tokens->items[0], pwd, strlen(pwd));
				strcat(tokens->items[0], "/bin/shell");
			}

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

			//TILDE EXPANSION (PART 3)
			//Checking if the token starts with a tilde
			if(tokens->items[i][0] == '~' && (tokens->items[i][1] == '\0' 
			|| tokens->items[i][1] == '/'))
			{
				//Get HOME evironment variable
				char *home = getenv("HOME");

				if(home)  //If $HOME is set
				{
					//Assign space for the expanded path
					//using malloc() to allocate the requested memory and return pointer to it
					char *expandedPath = (char *)malloc(strlen(home) 
					+ strlen(tokens->items[i]) + 1);
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
					errorMessage = 
					"ERROR: Tilde expansion failed HOME environment variable is not set.\n";
					error = true;
				}
			}

			//PATH SEARCH (PART 4)
			//GET THE WHOLE $PATH VARIABLE
			char * ptrPath = getenv("PATH");
			char path[256];
			strcpy(path, ptrPath);


			//delimit path variable by : (seperate directories)
			char* token = strtok(path, ":");
			//bool isExecutable = false;

			//PATH SEARCH (PART 4)
			bool wantToExpand = true;//PREVENT EXPANSION OF INTERNAL COMMANDS AND NONCOMMANDS
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
				
			//IF THE TOKEN IS NOT ALREADY AN EXECUTABLE PATH AND WE WANT TO EXPAND,
			//THEN DO PATH SEARCH (PT4) AND EXPAND EACH COMMAND
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
						tokens->items[i] = (char *)realloc(tokens->items[i], 
						strlen(executable) + 1);
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
			}
		//PT6-I/O REDIRECTION
		if((strcmp(tokens->items[i], "cmd")==0)||(strcmp(tokens->items[i], "CMD")==0))
			{
				//FILE OUT
				if(strcmp(tokens->items[i+1], ">")==0)
				{
					if(tokens->items[i+3]!= NULL)
					{		//MULTISTEP
						if(strcmp(tokens->items[i+3], "<")==0)
						{
							rediri = open(tokens->items[i+4], O_RDONLY);
							if(rediri == -1)
							{
								perror("The file requested does not exist or is not a regular file.");
							} else
							{
								dup2(rediri, STDIN_FILENO);
								isFileIn = 1;
								
							}
						}
					}
					rediro = open(tokens->items[i+2], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); //important note: logical or (||) will cause the program to panic. Use (|) for flag seperation.
					dup2(rediro, STDOUT_FILENO);
					isFileOut = 1;
					
				} //FILE IN
				else if(strcmp(tokens->items[i+1], "<")==0)
				{
					rediri = open(tokens->items[i+2], O_RDONLY);
					if(rediri == -1)
					{
						perror("The file requested does not exist or is not a regular file.");
					} else
					{
						dup2(rediri, STDIN_FILENO);
						isFileIn = 1;
						
					}
					if(tokens->items[i+3]!=NULL)
					{		//MULTISTEP
						if(strcmp(tokens->items[i+3], ">")==0)
						{
							rediro = open(tokens->items[i+4], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
							dup2(rediro, STDOUT_FILENO);
							isFileOut = 1;
							
						}
					}
				}

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
		case 1: //case one for guideline cmd1 | cmd2 (cmd1 redirects its standard output to 
		//the standard input of cmd2)
			if (pipe(pipe1) == -1) 
			{
            	perror("pipe1 failed");
            	exit(EXIT_FAILURE);
        	}
			//Uses the execute_cmd_with_path function to execute the two commands with appropriate
			// read and write pipes
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

		//THIS IS WHERE PREVIOUSLY INITIALIZED ERROR MESSAGES WILL BE DISPLAYED
		if(error)
		{
			//DISPLAY ANY ERROR MESSAGE HERE THAT HAPPENS BEFORE COMMAND EXECUTION
			printf("%s\n",errorMessage);
		}
		else
		{
			
			//CHECKS TO MAKE SURE COMMAND DOES NOT HAVE ANY <, >, OR | TOKENS
			//(SEPERATE CASES ELSEWHERE IN FILE FOR <, >, |)
			int IOorPipe = false;//SO THAT #5, 8, & 9 DO NOT INTERFERE WITH 6 & 7
			for(int i = 0; i < tokens->size; i++)
			{
				if(strcmp(tokens->items[i], "|") == 0 || strcmp(tokens->items[i], ">") == 0
				|| strcmp(tokens->items[i], "<") == 0)
					IOorPipe = true;
			}

			//(PART 9) BACKGROUND PROCESSING (EXCLUDING IO AND PIPE)
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
				else {//INCREMENT COUNT AND ADD TO JOB STRUCTURE FOR JOBS INTERNAL COMMAND (PART 8)
					jobCount++;//AND BACKGROUND EXECUTION (PART 9)
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
								strncpy(jobList[i].commandLine, tempcmd, 
								sizeof(jobList[i].commandLine));
								break;
							}
						}
					}
					else if(jobsRunning > 10)
					{
						error = true;
						printf("ERROR: maximum number of background jobs to display reached\n");
					}
					waitpid(pid, &status, WNOHANG);
				}
			}
			else if(IOorPipe == false)
			{
				if(strcmp(tokens->items[0], "exit") == 0)//EXIT INTERNAL COMMAND (PART 9)
				{
					if(totalCommandHistory == 3)//IF 3, PRINT LAST 3 JOBS (NO ORDER)
					{
						printf("Last (%d) valid commands:\n", totalCommandHistory);
						printf("[1]: %s\n", cmd0);
						printf("[2]: %s\n", cmd1);
						printf("[3]: %s\n", cmd2);
					}
					else if(totalCommandHistory > 0)//IF <3 & >0, PRINT AS MANY AS CAN FIND (1 OR 2)
					{
						printf("Last valid command(s):\n");
						if(totalCommandHistory%3 == 1)
							printf("[1]: %s\n", cmd0);
						else if(totalCommandHistory%3 == 2)
							printf("[1]: %s\n", cmd1);
						else if(totalCommandHistory%3 == 3)
							printf("[1]: %s\n", cmd2);
					}
					else//IF THERE ARE NONE, SAY SO
					{
						printf("No valid commands in history.\n");
					}
					exit(0);
				}
				else if(strcmp(tokens->items[0], "cd") == 0)//CD INTERNAL COMMAND EXECUTION (PART 9)
				{//IF TOO MANY ARGUMENTS, SAY SO
					if(tokens->size > 2 && strcmp(tokens->items[2], "|") != 0 &&
					strcmp(tokens->items[2], "<") != 0 && strcmp(tokens->items[2], ">") != 0)
					{
						error = true;
						printf("ERROR: too many arguments\n");
					}//IF NO ARGUMENTS, CHDIR TO $HOME
					else if(tokens->items[1] == NULL || strlen(tokens->items[1]) < 0)
					{
						chdir(getenv("HOME"));
					}//CHECK IF CHDIR WORKS (IF SO, CHDIR)
					else if(chdir(tokens->items[1]) != 0)
					{//CHECKS IF DIRECTORY EXISTS. IF NOT, SAY SO
						if(access(tokens->items[1], F_OK) != 0)
    						printf("ERROR: directory does not exist\n");
						else//IF NOT A DIRECTORY, SAYS SO
							printf("ERROR: not a directory\n");
						error = true;
					}
				}//JOBS INTERNAL COMMAND EXECUTION (PART 9)
				else if(strcmp(tokens->items[0], "jobs") == 0)
				{
					for (int i = 1; i <= 10; i++) //PRINTS OUT ALL THE JOBS (IN STRUCTURE)
					{
						if(jobList[i].jobNumber > 0)
							printf("[%d]+ %d %s\n", jobList[i].jobNumber, jobList[i].pid, jobList[i].commandLine);
					}
				}//PART 5 (NO PIPE OR IO)
				else if(access(tokens->items[0], F_OK) == 0 && access(tokens->items[0], X_OK) == 0)
				{
					int status;
					pid_t pid = fork();
					if (pid == 0)
					{
						execv(tokens->items[0], tokens->items);
					}
					else {
						waitpid(pid, &status, 0);
					}
				}
				else//IF NOT CD, JOBS, EXIT, BACKGROUND PROCESS, OR EXTERNAL COMMAND; PRINT ERROR
				{
					error = true;
					printf("ERROR: Command not found or not executable.\n");
				}
				
			}
		}

		//SAVES THE COMMAND HISTORY FOR EXIT INTERNAL COMMAND EXECUTION (PART 9)
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

		//FREE HERE
		free(input);
		free_tokens(tokens);
		free(pwd);
	}

	return 0;
}


//PROVIDED FUNCTIONS FOR INPUT PROCESSING

//GETS THE INPUT FROM THE COMMAND LINE
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

//MAKES AN EMPTY TOKENLIST
tokenlist *new_tokenlist(void) {
	tokenlist *tokens = (tokenlist *)malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->items = (char **)malloc(sizeof(char *));
	tokens->items[0] = NULL; /* make NULL terminated */
	return tokens;
}

//ADDS A TOKEN WITH SPECIFIED ITEM TO THE TOKENLIST PROVIDED 
void add_token(tokenlist *tokens, char *item) {
	int i = tokens->size;

	tokens->items = (char **)realloc(tokens->items, (i + 2) * sizeof(char *));
	tokens->items[i] = (char *)malloc(strlen(item) + 1);
	tokens->items[i + 1] = NULL;
	strcpy(tokens->items[i], item);

	tokens->size += 1;
}

//GETS THE TOKENS FROM AN INPUT (PARSES)
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

//FREES THE TOKENS
void free_tokens(tokenlist *tokens) {
	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);
	free(tokens->items);
	free(tokens);
}
