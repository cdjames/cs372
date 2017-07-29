/*********************************************************************
** Author: Collin James
** Date: 7/29/17
** Description: Class to implement connecting to, sending to, receiving from server (header)
** Uses code from cs344 server assignment (recvMsg, recvAll, sendAll)
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

	/* private variables */
	bool is_client,
		 is_server; // not used

private:
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
	** set the socket to have timeouts
	*********************************************************************/
	bool setTimeout(int s);
	bool setNonBlocking(int s);
	int sendAll(int s, const void * msg, string tosend, int *amountToSend);
	bool checkAndReceive(int s);
	int recvMsg(char * buf, int buf_len, int cnctFD);
	int recvAll(int socketFD, void * buf, int * amountToRecv);
	void clearString(char * theString, int size);
	void errorCloseSocket(const char *msg, int socketFD);
	void splitString(string str, vector<string> &container);
	void _cleanup();
	
	int clientSocket,
		listenSocket,
		connSocket,
		port;
	bool got_aitei_handle,
		 handle_sent;
	string aiteiHandle,
		   code,
		   handle,
		   host;
	mutex *inlock, *outlock;
	deque<string> *inq, *outq;
};

#endif