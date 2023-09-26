#define _POSIX_C_SOURCE 200809L
#include <features.h>
#include "lexer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *getPath(char *command)
{
  //check here if the command is executable
  if(access(command, X_OK) == 0)
  {
    return strdup(command); // Return a copy of the command
  }

  char *path, *pathCopy, *filePath, *token;
  path = getenv("PATH");   /*using getenv to search the environment list to find the specific environment variable name
                          it returns a pointer to the specified variable value or NULL if there is any match*/
  if (!path) return NULL;

  //making a copy of Path to avoid destroy the original
  pathCopy = strdup(path);
  if (!pathCopy) return NULL;  //if the copy fails return null

  int commandLength = strlen(command);  //get command length 

  token = strtok(pathCopy, ":"); // Tokenize by ':'
  
  while(token)
  {
    int directoryLength = strlen(token);  //get length of the current directory token obtained from $PATH
    //using malloc to dynamically allocate the memory 
    //adding command and directory path lengths + 2 for char. ('/') and null
    filePath = (char *)malloc(commandLength + directoryLength + 2);

    if(!filePath)
    {
      free(pathCopy);
      return NULL;
    }

    //constructing full path
    strcpy(filePath, token);   //Copy the directory to filePath
    strcat(filePath, "/");     
    strcat(filePath, command);  

    pid_t pid = fork();  
    if(pid == -1)
    {
      perror("fork");
      free(filePath);
      free(pathCopy);
      return NULL;
    }  

    if (pid == 0)  //child
    { 
      execv(filePath, (char * const []){command, NULL});
      exit(EXIT_FAILURE);  //exit if execv() fails
    } else {     // Parent
      int status;
      waitpid(pid, &status, 0);  //wait until child process has terminated
      if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
      {
        free(filePath);
      	if(token != NULL)
      	{
      		token = strtok(NULL, ":");
      	}
      } else 
        {
          free(pathCopy);
          return filePath;
        }
    }
  }
  free(pathCopy);
  return NULL;
}
