#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

//JOB STRUCTURE FOR jOBS INTERNAL COMMAND EXECUTION (PART 9)
struct Job {
    int jobNumber;
    pid_t pid;
    char commandLine[512];
};

//execute a given command with optional input and output redirection(part 7)
void execute_command(tokenlist* cmd, int input, int output) {
    //Variables to help in tokenizing the command and executing it
	//creating child process using fork()
   	if (fork() == 0) 
	{
        /*If the input file descriptor isn't the standard input(STDIN_FILENO),
          the child process's standard input is redirected to input*/
       if (input != STDIN_FILENO) 
		{
            dup2(input, STDIN_FILENO);
            close(input);
        }
		//same as STDIN but with output instead
        if (output != STDOUT_FILENO) 
		{
            dup2(output, STDOUT_FILENO);
            close(output);
        }
		//execv replaces the current child process's image with the new process image specified by the command in fullPath
        execv(cmd->items[0], cmd->items);
        exit(EXIT_FAILURE);
    }
}

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
	while (1) {
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

		if(tokens->size <= 0)
			continue;

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
				//printf("%s", tokens->items[i]);
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
		}

		//THIS IS WHERE PREVIOUSLY INITIALIZED ERROR MESSAGES WILL BE DISPLAYED
		if(error)
		{
			//DISPLAY ANY ERROR MESSAGE HERE THAT HAPPENS BEFORE COMMAND EXECUTION
			printf("%s\n",errorMessage);
		}
		else
		{
			//SEPERATE CASES FOR IOREDIRECT, PIPING, BACKGROUND, EXTERNAL, AND INTERNAL COMMANDS EX
			int IOorPipe = false;//TO KNOW WHEN TO EXECUTE BACKGORUND, EXTERNAL, AND INTERNAL
			int Pipe = false;//TO KNOW WHEN TO EXECUTE PIPING VERSUS IO REDIRECT
			for(int i = 0; i < tokens->size; i++)
			{
				if(strcmp(tokens->items[i], "|") == 0 || strcmp(tokens->items[i], ">") == 0
				|| strcmp(tokens->items[i], "<") == 0)
					IOorPipe = true;
				if(strcmp(tokens->items[i], "|") == 0)
					Pipe = true;
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
			else if(IOorPipe == true && Pipe == true)
			{
				//PARSING COMMANDS FOR FUTURE PIPES
				//find the index (indexes) of pipes
				int pipe1index = 0;//IF 0, NO PIPE, IF > 0 COULD BE 1 PIPE OR 2 PIPES
				int pipe2index = 0;//IF 0, LESS THAN 2 PIPES, IF 1, 2 PIPES
				for(int i = 0; i < tokens->size; i++)
				{
					if(strcmp(tokens->items[i], "|") == 0 && pipe1index == 0)
						pipe1index = i;
					else if (strcmp(tokens->items[i], "|") == 0 && pipe2index == 0)
						pipe2index = i;
				}
				//TESTING
				printf("pipe 1 index:%d\n", pipe1index);
				printf("pipe 2 index:%d\n", pipe2index);

				//iterate through all tokens and allocate each command (seperated by pipes)
				char command1[100] = "";
				char command2[100] = "";
				char command3[100] = "";
				for(int i = 0; i < tokens->size; i++)
				{
					if((i == pipe1index || i == pipe2index) && i != 0)
						continue;
					if(i < pipe1index && pipe1index != 0)
					{
						strcat(command1, tokens->items[i]);
						strcat(command1, " ");
					}
					else if((i < pipe2index && pipe2index != 0)|| (i < tokens->size && pipe1index != 0 
					&& pipe2index == 0))
					{
						strcat(command2, tokens->items[i]);
						strcat(command2, " ");
					}
					else if(pipe2index != 0)
					{
						strcat(command3, tokens->items[i]);
						strcat(command3, " ");
					}
				}
				//TESTING
				printf("command1: %s\n", command1);
				printf("command2: %s\n", command2);
				printf("command3: %s\n", command3);

				tokenlist *command1tokens = get_tokens(command1);
				tokenlist *command2tokens = get_tokens(command2);
				tokenlist *command3tokens = get_tokens(command3);

				//TESTING
				for(int i = 0; i < command1tokens->size; i++)
				{
					printf("command1tokens: %s ", command1tokens->items[i]);
				}
				printf("\n");
				for(int i = 0; i < command2tokens->size; i++)
				{
					printf("command2tokens: %s ", command2tokens->items[i]);
				}
				printf("\n");
				for(int i = 0; i < command3tokens->size; i++)
				{
					printf("command3tokens: %s ", command3tokens->items[i]);
				}
				printf("\n");

				//PART 7 PIPING
				//new part 7 test
				int pipe1[2], pipe2[2];
				//Create the first pipe
				if (pipe(pipe1) == -1) {
					perror("pipe1 failed");
					exit(EXIT_FAILURE);
				}

				//Execute the first command
				execute_command(command1tokens, STDIN_FILENO, pipe1[1]);
				close(pipe1[1]);

				//If there's a third command, create another pipe and set up for the second command
				if (pipe2index > 0) {//checks if pipe2index exists (there is a second pipe)
					if (pipe(pipe2) == -1) {
							perror("pipe2 failed");
							exit(EXIT_FAILURE);
				}
				execute_command(command2tokens, pipe1[0], pipe2[1]);
				close(pipe2[1]);
				close(pipe1[0]);

				//Execute the third command
				execute_command(command3tokens, pipe2[0], STDOUT_FILENO);
				close(pipe2[0]);
				} else {
				//If only two commands, directly set up for the second command
				execute_command(command2tokens, pipe1[0], STDOUT_FILENO);
				close(pipe1[0]);
				}

				wait(NULL);
				wait(NULL);
				if (pipe2index > 0) wait(NULL); //checks if pipe2index exists (there is a second pipe)
				//end of part 7 
			}
			else if(Pipe == false && IOorPipe == true)
			{
				//IO REDIRECTION
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
