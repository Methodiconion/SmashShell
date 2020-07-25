#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>
#include "history.h"

static int numberOfItems;
static int numericalIdentifier = 1;
static struct Cmd* history[MAXHISTORY];

void init_history() //I didn't know what to do here. I had trouble making it work when setting the global variables equal to the ones set in here, and in this example it really wasn't necessary so I didn't use it.
{
}
void add_history(char *cmd, int exitStatus)
{
  struct Cmd *h = malloc(sizeof(struct Cmd)); //allocates memory for the struct
  h->cmd = malloc(strlen(cmd) + 1); //allocates memory for the string
  strcpy(h->cmd, cmd);
  h->exitStatus = exitStatus;
  h->numericalIdentifier = numericalIdentifier;
  if(numberOfItems == MAXHISTORY)
  {
    struct Cmd *temp = history[0]; //stores access to the first node that is about to be removed so we can free it afterward.
    for(int i = 0; i <= 8; i++)
    {
      history[i] = history[i+1];
    }
    free(temp->cmd);
    free(temp);
    history[MAXHISTORY - 1] = h;
    numericalIdentifier++;
  }
  else
  {
    history[numberOfItems] = h;
    numericalIdentifier++; 
	numberOfItems++;
  }
}
void clear_history() //cycles through the array clearing all remaining entries.
{
	for(int i = 0; i < numberOfItems; i++)
	{
    free(history[i]->cmd);
		free(history[i]);
	}
}
void print_history(int firstSequenceNumber)
{
   if(numberOfItems - firstSequenceNumber < 0) //makes the indexing start at 0 if the range tries to push lower than that thereby preventing index out of bounds related crashes.
   {
     firstSequenceNumber = numberOfItems;
   }
   for(int i = (numberOfItems - firstSequenceNumber); i < numberOfItems; i++) //prints all the entires requested.
   {
     printf("%d%s%d%s %s\n", (*history[i]).numericalIdentifier, ". [",(*history[i]).exitStatus,"]",(*history[i]).cmd);
   }
}
