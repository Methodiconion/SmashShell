void init_history(void); 
void add_history(char *cmd, int exitStatus); 
void clear_history(void); 
void print_history(int firstSequenceNumber);

#define MAXHISTORY 10

struct Cmd
{
	char* cmd;
	int exitStatus;
  int numericalIdentifier;
};
