#include "Chatter.hpp"

Chatter::Chatter(int port, string host, string handle, deque<string> *inq, deque<string> *outq,
		mutex *inlock, mutex *outlock) {

	// clientSocket,
	// listenSocket,
	// connSocket
	this->inq = inq;
	this->outq = outq;
	this->port = port;
	this->host = host;
	got_aitei_handle = handle_sent = is_client = is_server = false;
	aiteiHandle = "";
	code = "namae:";
	this->handle = handle;
	this->inlock = inlock;
	this->outlock = outlock;
}
Chatter::~Chatter(){}

bool Chatter::connectToServer(){
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(this->port); // Store the port number
	serverHostInfo = gethostbyname(this->host.c_str()); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { 
		cerr << "CLIENT: ERROR, no such host\n" << endl;
		return false;
	}
	// Copy in the address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	// Set up the socket
	clientSocket = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (clientSocket < 0) { 
		cerr << "CLIENT: ERROR opening socket" << endl; 
		return false; 
	}

	// Connect to server
	int connected = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (connected < 0) {// Connect socket to address
		 cerr << "CLIENT: ERROR connecting" << endl; 
		 return false;
	}
	else
		cout << "connected!" << endl;
	
	// yay, you are connected!
	return true;
}