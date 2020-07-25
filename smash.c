#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>
#include <signal.h>
#include "smash.h"
#include "commands.c"

const int MAXLINE = 4096;
int c = 0;

void handler1(int sig)
{
	printf("%s", "\n");
  c = 1;
  signal(SIGINT, handler1);
}

int main(int argc, char **argv) 
{	
	signal(SIGINT, handler1);
	char bfr[MAXLINE];
	fputs("$ ",stderr);
	while(fgets(bfr, MAXLINE, stdin) != NULL || c == 1) 
	{
		bfr[strlen(bfr) - 1] = '\0'; //replace newline with NULex
		executeCommand(bfr);
		fputs("$ ",stderr);
	} 
	return 0;
}
