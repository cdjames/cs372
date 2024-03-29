/*********************************************************************
** Author: Collin James
** Date: 7/29/17
** Description: CS 372 Project 1: Class to implement connecting to, sending to, receiving from server (header)
** Uses code and ideas from cs344 server assignment (recvMsg, recvAll, sendAll)
** other sources attributed in chatclient.cpp
*********************************************************************/

#ifndef CHATTER_H
#define CHATTER_H
#include "includes.hpp"

class Chatter
{
public:
	/*********************************************************************
	** Description: Chatter()
	** set up most variables
	*********************************************************************/
	Chatter(int port, string host, string handle, deque<string> *inq, deque<string> *outq,
			mutex *inlock, mutex *outlock);
	~Chatter(); // not currently used

	/*********************************************************************
	** Description: connectToServer()
	** Try connecting to server
	** return false on error and true on success
	*********************************************************************/
	bool connectToServer();

	/*********************************************************************
	** Description: clientLoop()
	** Loop until "\quit" is received by server or socket connection is 
	** broken by peer/server
	*********************************************************************/
	void clientLoop();

	/* public variables */
	bool is_client,
		 is_server; // not used

private:
	/*********************************************************************
	** Description: _connect()
	** Params: socket FD, timeout in seconds, address info
	** Do the heavy lifting of connecting, including using select on non-
	** blocking socket
	** returns true on success and false on failure
	*********************************************************************/
	bool _connect(int s, int to, struct sockaddr_in serverAddress);

	/*********************************************************************
	** Description: sendHandle()
	** Params: socket FD
	** send your chat handle to the peer/server. returns number of chars written
	*********************************************************************/
	int sendHandle(int s);

	/*********************************************************************
	** Description: setTimeout()
	** Params: socket FD
	** set the socket to have timeouts on read and write (constants TO and TO_MS)
	** true on success and false on failure
	*********************************************************************/
	bool setTimeout(int s);

	/*********************************************************************
	** Description: setNonBlocking()
	** Params: socket FD
	** Make socket non-blocking so you can control timeout with select
	** true on success and false on failure
	*********************************************************************/
	bool setNonBlocking(int s);

	void gatherInput(); // !!! see chatclient.cpp for notes

	/*********************************************************************
	** Description: sendAll()
	** Params: socket FD, c string, c++ string (same as msg), length of string
	** send until you can't send any more!
	** returns -1 on error, 1 if no data is sent, 0 on success
	*********************************************************************/
	int sendAll(int s, const void * msg, string tosend, int *amountToSend);

	/*********************************************************************
	** Description: checkAndReceive()
	** Params: socket FD
	** Check if socket is ready to read from using select.
	** returns false if connection seems to be closed; true otherwise
	*********************************************************************/
	bool checkAndReceive(int s);

	/*********************************************************************
	** Description: recvMsg()
	** Params: c string, length of longest conceivable message (MAX_BUF), socket FD
	** Works in conjunction with recvAll to send a message
	** returns -1 if connection closed, 1 if no data sent, 0 on success
	*********************************************************************/
	int recvMsg(char * buf, int buf_len, int cnctFD);

	/*********************************************************************
	** Description: recvAll()
	** Params: socket FD, c string, pointer to MAX_BUF -1
	** attempts to send all data; same return values as recvMsg
	*********************************************************************/
	int recvAll(int socketFD, void * buf, int * amountToRecv);

	/* utility methods */
	/*********************************************************************
	** Description: clearString
	** convenience method for clearing c string via memset. mostly for readability
	*********************************************************************/
	void clearString(char * theString, int size);

	/*********************************************************************
	** Description: errorCloseSocket
	** Convenience method for printing a message and closing a socket
	*********************************************************************/
	void errorCloseSocket(const char *msg, int socketFD);
	
	/*********************************************************************
	** Description: 
	** got help here:
	Splitting strings:
	https://stackoverflow.com/questions/8448176/split-a-string-into-an-array-in-c
	https://stackoverflow.com/questions/11719538/how-to-use-stringstream-to-separate-comma-separated-strings#11719617
	*********************************************************************/
	void splitString(string str, vector<string> &container);
	
	/*********************************************************************
	** Description: _cleanup()
	** 
	*********************************************************************/
	void _cleanup();
	
	/* private variables */
	int clientSocket,
		listenSocket, // not used
		connSocket, // not used
		port; // keep track of the port
	bool got_aitei_handle, // tracks whether you have your conversant's handle
		 handle_sent; // tracks whether you have sent your handle
	string aiteiHandle, // keep your conversant's handle
		   code, // code for gathering handle
		   handle, // your handle
		   host; // keep track of the host IP
	deque<string> *inq,  // for shutting down input thread
				  *outq; // for getting messages from input thread
	mutex *inlock, *outlock; // for locking access to queues
};

#endif