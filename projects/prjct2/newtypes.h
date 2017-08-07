/*********************************************************************
** Author: Collin James
** Date: 11/9/16
** Description: Custom types used in smallsh
*********************************************************************/

#ifndef NEWT_H
#define NEWT_H
#include <stdlib.h> // for malloc
#include <string.h>  	// for memset
#include <unistd.h>		// for pid_t
#include <sys/types.h> 	// for struct stat

/* define the type for the deque */
# ifndef TYPE
# define TYPE      struct Pidkeeper
# define TYPE_SIZE sizeof(struct Pidkeeper)
# endif

# ifndef LT
# define LT(A, B) ((A) < (B))
# endif

# ifndef EQ
# define EQ(A, B) ((A) == (B))
# endif

/* for shortening unsigned int */
typedef unsigned int us_int;

/*********************************************************************
** Description: 
** Keeps the status of foreground processes. 
** Type: 0 = empty, 1 = exited, 2 = terminated
** sk_sig: -1 = init, otherwise could be 0 or more
*********************************************************************/
struct Statuskeeper
{
	us_int type;
	int sk_sig;
};

/*********************************************************************
** Description: 
** Intended to be an array of arguments (strings)
*********************************************************************/
struct argArray {
	char * arg;
};

/*********************************************************************
** Description: 
** Keeps details about the command the user wants to run; 1 = true, 0
** = false
*********************************************************************/
struct Commandkeeper 
{	
	us_int bg,		// 0/1 -- background?
		bltin,		// 0/1 -- built in function?
		red_in,		// 0/1 -- redirect in?
		red_out,	// 0/1 -- redirect out?
		red_error,	// 0/1 -- redirection error?
		no_cmd,		// not used	
		num_args;	// >= 0 -- # of arguments
	
	struct argArray * args; // any arguments
	char * cmd;				// the command
	char * infile;			// input file
	char * outfile;			// output file
};

/*********************************************************************
** Description: 
** For passing around pids and statuses (used in the queue in smallsh)
*********************************************************************/
struct Pidkeeper
{
	pid_t pid;
	int status;
};

/*********************************************************************
** Description: 
** Create a new Pidkeeper with a pid and status
*********************************************************************/
struct Pidkeeper new_PK(pid_t pid, int status);

/*********************************************************************
** Description: 
** Create a new Statuskeeper with a type and signal
*********************************************************************/
struct Statuskeeper * new_SK(us_int type, int sk_sig);

/*********************************************************************
** Description: 
** Create a new Commandkeeper with a command, an array of arguments,
** and the number of those arguments. Other values will default to 0 or
** NULL
*********************************************************************/
struct Commandkeeper new_CK(char * cmd, struct argArray * args, int num_args);

/*********************************************************************
** Description: 
** Free the memory of the SK pointer
*********************************************************************/
void free_sk(struct Statuskeeper * sk);

/*********************************************************************
** Description: 
** 0 = CONTINUE, 1 = EXIT
*********************************************************************/
enum Status { CONTINUE, EXIT };

#endif