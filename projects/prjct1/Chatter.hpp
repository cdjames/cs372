#ifndef CHATTER_H
#define CHATTER_H
#include "includes.hpp"

class Chatter
{
public:
	Chatter(int port, string host, string handle, deque<string> *inq, deque<string> *outq,
			mutex *inlock, mutex *outlock);
	~Chatter();

	bool connectToServer();

	void clientLoop();

	bool is_client,
		 is_server;

private:
	int sendHandle(int s);
	bool setTimeout(int s);
	bool setNonBlocking(int s);
	int sendAll(int s, const void * msg, string tosend, int *amountToSend);
	bool checkAndReceive(int s);
	int recvMsg(char * buf, int buf_len, int cnctFD);
	int recvAll(int socketFD, void * buf, int * amountToRecv);
	void clearString(char * theString, int size);
	void errorCloseSocket(const char *msg, int socketFD);
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