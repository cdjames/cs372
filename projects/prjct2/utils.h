/*********************************************************************
** Author: Collin James
** Date: 12/1/16
** Description: Utility functions used by otp_* and keygen (header)
*********************************************************************/

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h> 	// for stat()
#include <fcntl.h>		// for file manipulation
#include <sys/wait.h> 	// for wait/waitpid
#include <netinet/in.h>

#ifndef UTILS_H
#define UTILS_H

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
** Send all bytes or return error
w/ help from : https://beej.us/guide/bgnet/output/html/multipage/advanced.html
*********************************************************************/
int sendAll(int socketFD, void * msg, int * amountToSend);

int sendMsg(char * text, int cnctFD);

/*********************************************************************
** Description: 
** Receive all bytes or return error
w/ help from : https://beej.us/guide/bgnet/output/html/multipage/advanced.html
*********************************************************************/
int recvAll(int socketFD, void * buf, int * amountToRcv);

int recvMsg(char * buf, int buf_len, int cnctFD);
/*********************************************************************
** Description: 
** run perror(msg) and exit(1)
*********************************************************************/
void clearString(char * theString, int size);

/*********************************************************************
** Description: 
** Print a string to standard out, flushing buffer
*********************************************************************/
void printOut(char * outString, int newln);

/*********************************************************************
** Description: 
** Print a string to standard error, flushing buffer
*********************************************************************/
void printOutError(const char * outString, int newln);

/*********************************************************************
** Description: 
** Print a string and exit 1
*********************************************************************/
void error(const char *msg);

/*********************************************************************
** Description: 
** Print a string and exit 2
*********************************************************************/
void error2(const char *msg);

/*********************************************************************
** Description: 
** Print a string, close socket, and exit 1
*********************************************************************/
void errorCloseSocket(const char *msg, int socketFD);

/*********************************************************************
** Description: 
** Print a string and close socket (no exit)
*********************************************************************/
void errorCloseSocketNoExit(const char *msg, int socketFD);

/*********************************************************************
** Description: 
** Return a random integer from min to max (inclusive)
*********************************************************************/
int getRandom(int min, int max);

/*********************************************************************
** Description: 
** Check for invalid characters and return 1 if found; otherwise 0
*********************************************************************/
int hasValidChars(char * text);

/*********************************************************************
** Description: 
** Check text for invalid characters and exit if found (uses hasValidChars) 
*********************************************************************/
void checkText(char * text, int socketFD, char * fname);

/*********************************************************************
** Description: 
** Check for existence of file and exit if not found
*********************************************************************/
void checkFile(char * file, int socketFD);

/*********************************************************************
** Description: 
** Does both encryption and decryption of text in a child process.
** Returns a Pidkeeper with the pid and exit status
** cnctFD represents the socket
** PROG_CODE represents the connecting program's name
** PROG_NAME represents the running program's name
** hdShakeLen represents the length of the incoming handshake string
** dec: 0 means encrypt; 1 means decrypt
*********************************************************************/
struct Pidkeeper doEncryptInChild(int cnctFD, const char * PROG_CODE, const char * PROG_NAME, const int hdShakeLen, int dec);

/*********************************************************************
** Description: 
** Opens a listening socket and returns it; set previously used ports
** to be usable
** serverAddress stores info about the socket
** maxConn represents maximum number of connections
*********************************************************************/
int setUpSocket(struct sockaddr_in * serverAddress, int maxConn);

/*********************************************************************
** Description: 
** Used by doEncryptInChild to send an error status to the parent
*********************************************************************/
void sendErrorToParent(int pipe_status, int pipeFD, long int msg_size);

/*********************************************************************
** Description: 
** Return a modulus
w/ help: http://stackoverflow.com/questions/4003232/how-to-code-a-modulo-operator-in-c-c-obj-c-that-handles-negative-numbers#4003287
*********************************************************************/
int mod (int x, int y);

/*********************************************************************
** Description: 
** Encrypts or decrypts a string using the one time pad method
** the chars are self-explanatory
** dec: 0=encrypt, 1=decrypt
*********************************************************************/
void encrypt(char * instring, char * keystring, char * outstring, int dec);

#endif