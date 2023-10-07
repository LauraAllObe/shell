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

//JOB STRUCTURE FOR jOBS INTERNAL COMMAND EXECUTION (PART 9)
struct Job {
    int jobNumber;
    pid_t pid;
    char commandLine[512];
};

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
	bool isFileIn = 0;
	bool isFileOut = 0;
	int rediri = 0;//REDIRECT INPUT
	int rediro = 0; //REDIRECT OUTPUT- SAVE VARIABLE FOR LATER USE

	while (1) {
		/*
		if((isFileIn == 1) && (feof(stdin)))	//PT 6, SWITCH BACK CONTROL @ EOF
		{
			dup2(infd, STDIN_FILENO);			
			close(rediri);
			isFileIn = 0;
		}*/

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
		}

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
		//TESTING
		printf("io 1 index:%d\n", io1index);
		printf("io 2 index:%d\n", io2index);
		printf("io 1 is < (points left) is %d (1 = true/<, 0 = false/>)\n", io1pointsleft);
		printf("io 2 is < (points left) is %d (1 = true/<, 0 = false/>)\n", io2pointsleft);

		//iterate through all tokens and allocate each command (seperated by IO redirects)
		//THIS HOLDS THE VALUE OF THE COMMAND, "" IF NONE
		char comd1[100] = "";
		//THIS HOLDS THE VALUE OF THE FIRST FILE, "" IF NONE
		char file2[100] = "";
		//THIS HOLDS THE VALUE OF THE SECOND FILE, "" IF NONE
		char file3[100] = "";
		for(int i = 0; i < tokens->size; i++)
		{
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
		//TESTING
		printf("comd1: %s\n", comd1);
		printf("file2: %s\n", file2);
		printf("file3: %s\n", file3);

		tokenlist *commandTokens = get_tokens(comd1);

		//TESTING
		for(int i = 0; i < commandTokens->size; i++)
		{
			printf("command1tokens: %s ", commandTokens->items[i]);
		}
		printf("\n");

		int infd = dup(STDIN_FILENO);
		int outfd = dup(STDOUT_FILENO);

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
						close(STDIN_FILENO);
						rediri = open(file3, O_RDONLY);
						int status;
						pid_t pid = fork();
						if (pid == 0)
						{
							execv(commandTokens->items[0], commandTokens->items);
						}
						else {
							waitpid(pid, &status, 0);
						}

						if(rediri == -1)
						{
							perror("The file requested does not exist or is not a regular file.");
						} else
						{
							dup2(rediri, STDIN_FILENO);
							close(rediri);
							isFileIn = 1;
							
						}
					}
				}
				close(STDOUT_FILENO);
				rediro = open(file2, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); //important note: logical or (||) will cause the program to panic. Use (|) for flag seperation.
				
				if(io2index == 0)
				{
					int status;
					pid_t pid = fork();
					if (pid == 0)
					{
						execv(commandTokens->items[0], commandTokens->items);
					}
					else {
						waitpid(pid, &status, 0);
					}
				}
				
				dup2(rediro, STDOUT_FILENO);
				close(rediro);
				isFileOut = 1;
				
			} //FILE IN
			else if(io1pointsleft == true)
			{
				rediri = open(file2, O_RDONLY);
				if(rediri == -1)
				{
					perror("The file requested does not exist or is not a regular file.");
				} else
				{
					dup2(rediri, STDIN_FILENO);
					isFileIn = 1;
				}
				if(io2index > 0)
				{		//MULTISTEP
					if(io2pointsleft == true)
					{
						rediro = open(file3, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
						dup2(rediro, STDOUT_FILENO);
						isFileOut = 1;
					}
				}
			}
			/*
			int status;
			pid_t pid = fork();
			if (pid == 0)
			{
				execv(comd1->items[0], comd1->items);
			}
			else {
				waitpid(pid, &status, 0);
			}*/

		}

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
