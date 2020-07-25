#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "history.c"

int pipelocation[1000]; //pipe locations
int ioin[1000]; //io redirectors location
int ioout[1000]; //io redirectors location
int pipeNumber; //number of pipes found
int pipeInc;
int fd[2];
int fd2;
int other; //only used to keep track of a specific token tracker in the piping section.
int i;
int saved_stdout;
int saved_stdin;
pid_t pid1 = -1;

void handler2(int sig)
{
	printf("%s", "\n");
  signal(SIGINT, handler2);
}

void executeCommand(char *str)
{
	signal(SIGINT, handler2);
	saved_stdout = dup(1);
	saved_stdin = dup(0);
	pipeNumber = 0;
	pipeInc = 0;
	other = 0;
    init_history(); //pointless call in my implementation but it was required.
    int errorThrown = 0; //1 for true, 0 for false, 127 for exterior command, and 2 for successfully completion of external command.
    
    char *history = malloc(strlen(str) + 1); //a temporary string to hold a copy of the command before being broken down.
    strcpy(history, str); //copies the string of the command into the history temp string.
	
	char *token[1025];
	token[0] = strtok(str," ");
	i = 1;
	pipelocation[0] = -1;
	while (token[i - 1] != NULL && i <= 400) 
	{
		token[i] = strtok(NULL, " ");
		if(strcmp(token[i - 1], "|") == 0)
		{
			pipelocation[pipeNumber + 1] = (i - 1);
			pipeNumber++;
		}
		i++;
	}
	pipelocation[pipeNumber + 1] = (i - 1);
	
	if(token[0] != NULL)
	{
	do
	{
		if(pipeInc <= pipeNumber && pipeNumber != 0)
		{
			printf("%d \n", pipeNumber);
			char *tempList1[1025];
			int y = 0;
			for(int inc = pipelocation[other] + 1; inc < pipelocation[other + 1]; inc++) //splits all of the tokens between the pipes into their own string lists.
			{
				tempList1[y] = token[inc];
				if(strchr(tempList1[y], '<') != NULL)
				{
					tempList1[y] = NULL;
				}
				y++;
			}
			other++;
			if(pipeInc != pipeNumber + 1) //only reads in data if a previous command has already outed data via pipe.
			{
				fd2 = fd[0];
				pipe(fd);
			}
			
			pid1 = fork();
			if(pid1 == 0)
			{
        signal(SIGINT, SIG_DFL);
				if(pipeInc >= 1)
				{
					dup2(fd2, 0);
					close(fd2);
				}
				else //checks for stdin for piped command
				{
					char *tempList2[1025];
					int w = 0;
					for(int inc2 = pipelocation[pipeNumber] + 1; inc2 < pipelocation[pipeNumber + 1]; inc2++) //splits all of the tokens between the pipes into their own string lists.
					{
						tempList2[w] = token[inc2];
						w++;
					}
					int listindex = -1;
					int j = 0;
					while(tempList2[j] != NULL)
					{
						listindex++;
						j++;
					}
					if(strchr(tempList2[listindex], '<') != NULL)
					{
						if((strchr(tempList2[listindex], '<') - tempList2[listindex]) == 0)
						{
							tempList2[listindex] = strtok(tempList2[listindex], "<");
							int desc = open(strtok(tempList2[listindex], "<"), O_RDONLY); //removes the < from the string for opening the file
							if(desc != -1)  
							{
								dup2(desc, 0);
								close(desc);
							}
							else
							{
								exit(1);
							}
						}
					}
				}
				if(pipeInc != pipeNumber) //only pipe if there isn't an io out redirector
				{
					dup2(fd[1], 1);
					close(fd[1]);
					close(fd[0]);
				}
				else //planning for io redirection outward at the very end
				{
					int listindex = -1;
					int j = 0;
					while(tempList1[j] != NULL)
					{
						listindex++;
						j++;
					}
					if(strchr(tempList1[listindex], '>') != NULL)
					{
						if((strchr(tempList1[listindex], '>') - tempList1[listindex]) == 0)
						{
							tempList1[listindex] = strtok(tempList1[listindex], ">");
							int desc = open(strtok(tempList1[listindex], ">"), O_WRONLY |  O_TRUNC | O_CREAT, 0700); //removes the > from the string for opening the file
							if(desc != -1)  
							{
								dup2(desc, 1);
								close(desc);
							}
						}
						tempList1[listindex] = NULL;
					}
				}
				if(strcmp(tempList1[0], "history") == 0) //behavoir for the history command.
				{
					print_history(10);
					exit(0);
				}
				else
				{
					execvp(tempList1[0], tempList1);
					exit(127);
				}
			}
			else
			{
				if(pipeInc >= 1)
				{
					close(fd2);
				}
				close(fd[1]);
				wait(0);
				pipeInc++;
			}
			for(int i = 0; i <= 1025; i++)
			{
				tempList1[i] = NULL;
			}
		}
		
		if(pipeNumber == 0) //handles io redirection outward to file if only one command.
		{
			int listindex = -1;
			int j = 0;
			while(token[j] != NULL)
			{
				listindex++;
				if(strchr(token[listindex], '>') != NULL || strchr(token[listindex], '<') != NULL)
				{
					if((strchr(token[listindex], '>') - token[listindex]) == 0)
					{
						token[listindex] = strtok(token[listindex], ">");
						int desc = open(strtok(token[listindex], ">"), O_WRONLY |  O_TRUNC | O_CREAT, 0700); //removes the > from the string for opening the file
						if(desc != -1)  
						{
							dup2(desc, 1);
							close(desc);
						}
					}
					if((strchr(token[listindex], '<') - token[listindex]) == 0)
					{
						token[listindex] = strtok(token[listindex], "<");
						int desc = open(strtok(token[listindex], "<"), O_RDONLY); //removes the > from the string for opening the file
						if(desc != -1)  
						{
							dup2(desc, 0);
							close(desc);
						}
						else
						{
							char *output = token[listindex];
							strcat(output, ": ");
							fputs(output, stderr);
							errorThrown = 1;
							break;
						}
					}
				token[listindex] = NULL;
				}
			j++;
			}
		}
		if(strcmp(token[0], "exit") == 0)
		{
			clear_history();
			free(history); //frees the history string because it isn't used and needs to be freed early.
			exit(0);
		}
		else if(strcmp(token[0], "history") == 0) //behavoir for the history command.
		{
			if(token[1] != NULL) //passes the value if it was specified
			{
				print_history(atoi(token[1]));
			}
			else //passes the default max value if it was not specified.
			{
				print_history(10);
			}
		}
		else if(strcmp(token[0], "cd") == 0) //behavoir for the change directory command.
		{
			if(token[1] == NULL)
			{
				printf("Error: Invalid command.\n");
			}
			else if(chdir(token[1]) != 0)
			{
				errorThrown = 1;
				char *output = token[1];
				strcat(output, ": ");
				fputs(output, stderr);
			}
		}
		else if(pipeNumber == 0 && errorThrown != 1) //external commands
		{
			int temp;
			pid_t pid3 = fork();
			if(pid3 == -1)
			{
				errorThrown = 1;
			}
			else if(pid3 > 0)
			{
				wait(&temp);
			}
			else
			{
				execvp(token[0], token);
				exit(127); //sets up commands not being found.
			}
			errorThrown = WEXITSTATUS(temp);
		}
		if(errorThrown == 1) //reports any errors.
		{
			perror("");
		}
	} while(token[0] != NULL && pipeInc <= pipeNumber && pipeNumber != 0);
	}
	add_history(history, errorThrown); //adds the command to the history
	free(history); //frees the temporary command string
	dup2(saved_stdout, 1); //resets file descriptor
	dup2(saved_stdin, 0);
}
