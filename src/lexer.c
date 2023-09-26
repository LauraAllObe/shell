#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	while (1) {
		//ENVIRONMENTAL VARIABLES FOR PROMPT (PT1)
		const char *user = getenv("USER");
		const char *machine = getenv("MACHINE");
		const char *pwd = getenv("PWD");


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
		//printf("whole input: %s\n", input);

		tokenlist *tokens = get_tokens(input);
		for (int i = 0; i < tokens->size; i++) {
			//printf("token %d: (%s)\n", i, tokens->items[i]);
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

				//TESTING
				//printf("This is the environmental variable:%s\n",variable);


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

					//TESTING
					//printf("This is the new token: %s\n", tokens->items[i]);
				}

				//TESTING
				//printf("This is the variable: %s\n", envvar);
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
			//printf("Path variable is: %s\n",path);
			//delimit path variable by : (seperate directories)
			char* token = strtok(path, ":");
			char finalExecutable[256];
			bool isExecutable = false;
			//check if the token is executable (it is already a path)
			if(access(tokens->items[i], F_OK) != 0 || access(tokens->items[i], X_OK) != 0)
			{
				//iterates through $PATH directories seperated by :
				while(token != NULL)
				{
					//concatenate each $PATH directory with the command/token
					char executable[256];
					//printf("Tokenized path without executable: %s\n", token);
    				strcpy(executable, token); // Copy the first part
    				strcat(executable, "/"); // Concatenate a forward slash
    				strcat(executable, tokens->items[i]);
					//printf("Tokenized path with executable: %s\n", executable);
					//if it is executable, copy to finalExecutable
					if(access(executable, F_OK) == 0 && access(executable, X_OK) == 0)
					{
						isExecutable = true;
						strcpy(finalExecutable, executable);
					}
					token = strtok(NULL, ":");
				}
				//if it is executable, copy final executable into token item
				if(isExecutable)
				{
					tokens->items[i] = (char *)realloc(tokens->items[i], strlen(finalExecutable) + 1);
					strcpy(tokens->items[i], finalExecutable);
					//printf("~~New token:%s\n",tokens->items[i]);
				}
				else
				{
					errorMessage = "ERROR: Command not found or not executable.\n";
					error = true;
				}
			}
			
		}

		if(error)
		{
			//DISPLAY ANY ERROR MESSAGE HERE THAT HAPPENS BEFORE COMMAND EXECUTION
			printf("%s\n",errorMessage);
		}
		else
		{
			//SPACE FOR FUTURE COMMANDS,ETC
		}

		free(input);
		free_tokens(tokens);
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