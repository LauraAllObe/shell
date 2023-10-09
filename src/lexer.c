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

//used for error checking
bool error = false;

//JOB STRUCTURE FOR jOBS INTERNAL COMMAND EXECUTION (PART 9)
struct Job {
    int jobNumber;
    pid_t pid;
    char commandLine[512];
};

//used for background execution
int jobCount;
int jobsRunning;
struct Job jobList[10];

//execute a given command with optional input and output redirection(part 7)
void execute_command(tokenlist* cmd, int input, int output, bool isBackground, char* tempcmd, bool 
isLast) {
    //Variables to help in tokenizing the command and executing it
	//creating child process using fork()
	int status;
	pid_t pid = fork();
   	if (error == false && pid == 0) 
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
		execv(cmd->items[0], cmd->items);
        //exit(EXIT_FAILURE);
    }
	if(error == false && isBackground && pid != 0 && isLast)
	{
		//INCREMENT COUNT AND ADD TO JOB STRUCTURE FOR JOBS INTERNAL COMMAND (PART 8)
		jobCount++;//AND BACKGROUND EXECUTION (PART 9)
		jobsRunning++;
		printf("[%d] [%d]\n", jobCount, pid);
		if(jobsRunning <= 10)
		{
			for(int i = 0; i < 10; i++)
			{
				if(jobList[i].jobNumber == 0)
				{
					jobList[i].jobNumber = jobCount;
					jobList[i].pid = pid;
					strncpy(jobList[i].commandLine, tempcmd, 
					sizeof(jobList[i].commandLine));
					break;
				}
			}
		}
		else
		{
			error = true;
			printf("ERROR: maximum number of background jobs to display reached\n");
		}
	}
	if(isBackground && pid != 0)
	{
		waitpid(pid, &status, WNOHANG);
	}
}

int main()
{	
	jobCount = 0;
	jobsRunning = 0;
	//FOR PART 9 INTERNAL COMMAND EXECUTION JOBS, INITIALIZE JOB LIST JOB # TO 0
	for(int i = 0; i < 10; i++)
	{
		jobList[i].jobNumber = 0;
		jobList[i].pid = 0;
		strncpy(jobList[i].commandLine, "", sizeof(jobList[i].commandLine));
	}

	//FOR PART 9 INTERNAL COMMAND EXECUTION EXIT, INITIALIZE PLACEHOLDERS AND 
	int commandHistory = 0;//COUNTERS FOR THE THREE MOST RECENT COMMANDS
	int totalCommandHistory = 0;
	char *cmd0 = (char *)malloc(sizeof(char *) * 200);
	char *cmd1 = (char *)malloc(sizeof(char *) * 200);
	char *cmd2 = (char *)malloc(sizeof(char *) * 200);
	char *tempcmd = (char *)malloc(sizeof(char *) * 200);

	int rediri = 0;
	int rediro = 0;
	while (1) {
		error = false;
		size_t size = 0;
		size = pathconf(".", _PC_PATH_MAX);
		//ENVIRONMENTAL VARIABLES FOR PROMPT (PART 1)
		const char *user = getenv("USER");
		const char *machine = getenv("MACHINE");
		char *pwd = NULL;		
		if ((pwd = (char *)malloc((size_t)size)) != NULL) 
        	getcwd(pwd, (size_t)size);


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

			bool wantToExpand = true;//PREVENT EXPANSION OF INTERNAL COMMANDS AND NONCOMMANDS
			//FOR part 10 (mytimeout)
			if(strcmp(tokens->items[i], "./bin/mytimeout") == 0)
			{
				tokens->items[0] = (char *)realloc(tokens->items[0], strlen(pwd) + 1);
				strncpy(tokens->items[0], pwd, strlen(pwd));
				strcat(tokens->items[0], "/bin/mytimeout");
				wantToExpand = false;
			}

			//IF CURRENT TOKEN IS AN ENVIRONMENTAL VARIABLE
			if(tokens->items[i][0]=='$')
			{				
				char variable[50];
				strncpy(variable, tokens->items[i] + 1, strlen(tokens->items[i]));
				variable[strlen(variable)] = '\0';

				const char *envvar = getenv(variable);

				//ERROR CHECKING FOR BAD ENVIRONMENTAL VARIABLE
				if(envvar == NULL)
				{
					printf("ERROR: BAD ENVIRONMENTAL VARIABLE\n");
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
					printf("ERROR: Tilde expansion failed HOME environment variable is not set.\n");
					error = true;
				}
			}
			else if(tokens->items[i][0] == '~' && !(tokens->items[i][1] == '\0' 
			|| tokens->items[i][1] == '/'))
			{
				printf("ERROR: Tilde expansion failed (not standalone or ~/).\n");
				error = true;
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
						strcpy(tokens->items[i], executable);
						break;	
					}
					token = strtok(NULL, ":");
				}
			}
		}

		if(!error)
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
					printf("[%d] [%d]\n", jobCount, pid);
					if(!error && (jobsRunning <= 10))
					{
						for(int i = 0; i < 10; i++)
						{
							if(jobList[i].jobNumber == 0)
							{
								jobList[i].jobNumber = jobCount;
    							jobList[i].pid = pid;
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
					//DECREMENT BACKGROUND PROCCESSES WHEN COMPLETED FOR BACKGROUND PROCESSING (PART 8) AND
					for(int i = 0; i < 10; i++)//JOBS INTERNAL COMMAND EXECUTION (PART 9)
					{
						pid_t pid = 0;
						if(jobList[i].jobNumber > 0)
							pid = waitpid(jobList[i].pid, NULL, WNOHANG);
						if(pid <= 0 && jobList[i].jobNumber > 0)
						{
							printf("You must wait for all processeses to finish first.\n");
							error = true;
							break;
						}
					}
					if(!error && totalCommandHistory >= 3)//IF 3, PRINT LAST 3 JOBS (NO ORDER)
					{
						printf("Last (3) valid commands:\n");
						printf("[1]: %s\n", cmd0);
						printf("[2]: %s\n", cmd1);
						printf("[3]: %s\n", cmd2);
					}
					else if(!error && totalCommandHistory > 0)//IF <3 & >0, PRINT AS MANY AS CAN FIND (1 OR 2)
					{
						printf("Last valid command(s):\n");
						if(totalCommandHistory%3 == 1)
							printf("[1]: %s\n", cmd0);
						else if(totalCommandHistory%3 == 2)
							printf("[1]: %s\n", cmd1);
						else if(totalCommandHistory%3 == 3)
							printf("[1]: %s\n", cmd2);
					}
					else if(!error)//IF THERE ARE NONE, SAY SO
					{
						printf("No valid commands in history.\n");
					}
					if(!error)
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
    						printf("ERROR: not a directory or directory does not exist\n");
						else//IF NOT A DIRECTORY, SAYS SO
							printf("ERROR: not a directory\n");
						error = true;
					}
				}//JOBS INTERNAL COMMAND EXECUTION (PART 9)
				else if(strcmp(tokens->items[0], "jobs") == 0)
				{
					bool none = true;
					for (int i = 0; i < 10; i++) //PRINTS OUT ALL THE JOBS (IN STRUCTURE)
					{
						if(jobList[i].jobNumber > 0)
						{
							printf("[%d]+ [%d] [%s]\n", jobList[i].jobNumber, jobList[i].pid, 
							jobList[i].commandLine);
							none = false;
						}
					}
					if(none)
						printf("No active background processes.\n");
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

				//iterate through all tokens and allocate each command (seperated by pipes)
				char command1[100] = "";
				char command2[100] = "";
				char command3[100] = "";

				bool backgroundprocess= false;

				for(int i = 0; i < tokens->size; i++)
				{
					//FOR BACKGROUND PROCESSES
					if(i == tokens->size-1 && tokens->items[tokens->size -1][0] == '&')
					{
						backgroundprocess = true;
						break;
					}
					if((i == pipe1index || i == pipe2index) && i != 0)
						continue;
					if(i < pipe1index && pipe1index != 0)
					{
						strcat(command1, tokens->items[i]);
						strcat(command1, " ");
					}
					else if((i < pipe2index && pipe2index != 0)|| 
					(i < tokens->size && pipe1index != 0 && pipe2index == 0))
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

				tokenlist *command1tokens = get_tokens(command1);
				tokenlist *command2tokens = get_tokens(command2);
				tokenlist *command3tokens = get_tokens(command3);

				//PART 7 PIPING
				//new part 7 test
				int pipe1[2], pipe2[2];
				//Create the first pipe
				if (pipe(pipe1) == -1) {
					printf("pipe1 failed");
					error = true;
					exit(EXIT_FAILURE);
					continue;
				}

				//Execute the first command
				execute_command(command1tokens, STDIN_FILENO, pipe1[1], backgroundprocess, tempcmd,
				false);//false because is not last command (for background exec)
				close(pipe1[1]);

				//If there's a third command, create another pipe and set up for the second command
				if (pipe2index > 0) {//checks if pipe2index exists (there is a second pipe)
					if (pipe(pipe2) == -1) {
						printf("pipe2 failed");
						exit(EXIT_FAILURE);
						error = true;
						continue;
					}
					execute_command(command2tokens, pipe1[0], pipe2[1], backgroundprocess, tempcmd, 
					false);//false because is not last command (for background exec)
					close(pipe2[1]);
					close(pipe1[0]);

					//Execute the third command
					execute_command(command3tokens, pipe2[0], STDOUT_FILENO, 
					backgroundprocess, tempcmd, true);//true because is last (for background exec)
					close(pipe2[0]);
				} else {
					//If only two commands, directly set up for the second command
					execute_command(command2tokens, pipe1[0], STDOUT_FILENO, 
					backgroundprocess, tempcmd, true);//true because is last (for background exec)
					close(pipe1[0]);
				}
				if(!backgroundprocess)
				{
					wait(NULL);
					wait(NULL);
					if (pipe2index > 0) wait(NULL); //checks if pipe2index exists (there is a second pipe)
				}
			}
			else if(Pipe == false && IOorPipe == true)
			{
				//PARSING COMMANDS FOR FUTURE IO REDIRECTION
				//THIS INDICATES THE INDEX LOCATION OF THE FIRST < OR > SYMBOL
				int io1index = 0;//IF GREATER THAN 0, IT IS TRUE THAT IO REDIRECTION IS TAKING PLACE
				//THIS INDICATES THE INDEX LOCATION OF THE SECOND < OR > SYMBOL
				int io2index = 0;//IF GREATER THAN 0, IT IS TRUE THAT TWO IO REDIRECTIONS ARE TAKING PLACE
				//IF TRUE, THIS INDICATES THAT IO REDIRECTION #1 IS <, > IF FALSE
				bool io1pointsleft = false;
				//IF TRUE, THIS INDICATES THAT IO REDIRECTION #2 IS <, > IF FALSE
				bool io2pointsleft = false;
				//ALL OF THE ABOVE VALUES CAN BE USED FOR CASE CHECKING!!
				bool IO = false;
				for(int i = 0; i < tokens->size; i++)
				{
					if(strcmp(tokens->items[i], "<") == 0 &&  io1index == 0)
					{
						IO = true;
						io1index = i;
						io1pointsleft = true;
					}
					else if(strcmp(tokens->items[i], ">") == 0 &&  io1index == 0)
					{
						IO = true;
						io1index = i;
					}
					else if (strcmp(tokens->items[i], "<") == 0 && io2index == 0)
					{
						IO = true;
						io2index = i;
						io2pointsleft = true;
					}
					else if (strcmp(tokens->items[i], ">") == 0 && io2index == 0)
					{
						IO = true;
						io2index = i;
					}
				}
		
				//check if background process (FOR IO Redirection)
				bool background;
				if(strcmp(tokens->items[tokens->size-1], "&") == 0 && IO == true)
				{
					background = true;
				}
		
				//iterate through all tokens and allocate each command (seperated by IO redirects)
				//THIS HOLDS THE VALUE OF THE COMMAND, "" IF NONE
				char comd1[100] = "";
				//THIS HOLDS THE VALUE OF THE FIRST FILE, "" IF NONE
				char file2[100] = "";
				//THIS HOLDS THE VALUE OF THE SECOND FILE, "" IF NONE
				char file3[100] = "";
				for(int i = 0; i < tokens->size; i++)
				{
					if(i == tokens->size - 1 && background)
						break;
					if((i == io1index || i == io2index) && i != 0)
						continue;
					if(i < io1index && io1index != 0)
					{
						strcat(comd1, tokens->items[i]);
						strcat(comd1, " ");
					}
					else if((i < io2index && io2index != 0)|| (i < tokens->size && io1index != 0 
					&& io2index == 0))
					{
						strcat(file2, tokens->items[i]);
						strcat(file2, " ");
					}
					else if(io2index != 0)
					{
						strcat(file3, tokens->items[i]);
						strcat(file3, " ");
					}
				}
		
				tokenlist *commandTokens = get_tokens(comd1);
				int infd;
				int outfd;
				
				//PT6-I/O REDIRECTION
				if(io1index > 0 && IO == true)
				{
					//FILE OUT
					if(io1pointsleft == false)
					{
						if(io2index > 0)
						{		//MULTISTEP
							if(io2pointsleft == true)
							{
								infd = dup(STDIN_FILENO);
								close(STDIN_FILENO);
								rediri = open(file3, O_RDONLY);
								if(rediri == -1)
								{
									error = true;
									printf("The file requested does not exist or is not regular\n");
									close(rediri);
									dup2(infd, STDIN_FILENO);
									close(infd);
									continue;
								}
							}
						}
						if(rediri == -1)
							continue;
						outfd = dup(STDOUT_FILENO);
						close(STDOUT_FILENO);
						rediro = open(file2, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
						
						int status;
						pid_t pid = fork();
						if(rediri != -1 && background == false)
						{
							if (pid == 0)
							{
								execv(commandTokens->items[0], commandTokens->items);
							}
							else {
								waitpid(pid, &status, 0);
							}
						}
						else if(rediri != -1 && background == true)
						{
							if(pid == 0) {
								if (access(commandTokens->items[0], X_OK) == 0)
								{
									execv(commandTokens->items[0], commandTokens->items);
								}
								else
								{
									printf("ERROR: Command not found or not executable.\n");
									error = true;
									continue;
								}
							}
							else {//INCREMENT COUNT AND ADD TO JOB STRUCTURE FOR JOBS (PART 8)
								jobCount++;//AND BACKGROUND EXECUTION (PART 9)
								jobsRunning++;
								if(!error && (jobsRunning <= 10))
								{
									for(int i = 0; i < 10; i++)
									{
										if(jobList[i].jobNumber == 0)
										{
											jobList[i].jobNumber = jobCount;
											jobList[i].pid = pid;
											strncpy(jobList[i].commandLine, tempcmd, 
											sizeof(jobList[i].commandLine));
											break;
										}
									}
								}
								else if(jobsRunning > 10)
								{
									error = true;
									printf("ERROR: maximum number of jobs to display reached\n");
								}
								waitpid(pid, &status, WNOHANG);
							}
						}
						
						if(io2index > 0 && io2pointsleft == true)
						{
							dup2(infd, STDIN_FILENO);
							close(infd);
						}
						dup2(outfd, STDOUT_FILENO);
						close(outfd);
						if(background)
							printf("[%d] [%d]\n", jobCount, pid);
					} //FILE IN
					else if(io1pointsleft == true)
					{
						infd = dup(STDIN_FILENO);
						close(STDIN_FILENO);
						rediri = open(file2, O_RDONLY);
						if(rediri == -1)
						{
							error = true;
							printf("The file requested does not exist or is not regular.\n");
							dup2(infd, STDIN_FILENO);
							close(infd);
							continue;
						}
						if(rediri == -1)
							continue;
						if(io2index > 0)
						{		//MULTISTEP
							if(io2pointsleft == false)
							{
								outfd = dup(STDOUT_FILENO);
								close(STDOUT_FILENO);
								rediro = open(file3, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
							}
						}
		
						int status;
						pid_t pid = fork();
						if(rediri != -1 && background == false)
						{
							if (pid == 0)
							{
								execv(commandTokens->items[0], commandTokens->items);
							}
							else {
								waitpid(pid, &status, 0);
							}
						}
						else if(rediri != -1 && background == true)
						{
							if(pid == 0) {
								if (access(commandTokens->items[0], X_OK) == 0)
								{
									execv(commandTokens->items[0], commandTokens->items);
								}
								else
								{
									printf("ERROR: Command not found or not executable.\n");
									error = true;
									continue;
								}
							}
							else {//INCREMENT COUNT AND ADD TO JOB STRUCTURE FOR JOBS (PART 8)
								jobCount++;//AND BACKGROUND EXECUTION (PART 9)
								jobsRunning++;
								if(!error && (jobsRunning <= 10))
								{
									for(int i = 0; i < 10; i++)
									{
										if(jobList[i].jobNumber == 0)
										{
											jobList[i].jobNumber = jobCount;
											jobList[i].pid = pid;
											strncpy(jobList[i].commandLine, tempcmd, 
											sizeof(jobList[i].commandLine));
											break;
										}
									}
								}
								else if(jobsRunning > 10)
								{
									error = true;
									printf("ERROR: maximum number of jobs to display reached\n");
								}
								waitpid(pid, &status, WNOHANG);
							}
						}
						if(error == true)
							continue;
						dup2(infd, STDIN_FILENO);
		                close(infd);
		
		                if(io2index > 0)
		                {        //MULTISTEP
		                    if(io2pointsleft == false)
		                    {
		                        dup2(outfd, STDOUT_FILENO);
		                        close(outfd);
		                    }
		                }
		
						if(background)
							printf("[%d] [%d]\n", jobCount, pid);
					}
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

		//DECREMENT BACKGROUND PROCCESSES WHEN COMPLETED FOR BACKGROUND PROCESSING (PART 8) AND
		for(int i = 0; i < 10; i++)//JOBS INTERNAL COMMAND EXECUTION (PART 9)
		{
			pid_t pid = 0;
			if(jobList[i].jobNumber > 0)
				pid = waitpid(jobList[i].pid, NULL, WNOHANG);
			if(pid > 0)
			{
				jobList[i].jobNumber = 0;
				jobList[i].pid = 0;
				jobsRunning--;
				printf("[Job %d] + done [%s]\n", pid, jobList[i].commandLine);
			}
		}

		//FREE HERE
		free(input);
		free_tokens(tokens);
		free(pwd);
	}
	free(cmd0);
	free(cmd1);
	free(cmd2);
	free(tempcmd);

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
