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

	bool is_client,
		 is_server;

private:
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