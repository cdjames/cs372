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
	
	is_client = true;
	// yay, you are connected!
	return true;
}

int Chatter::sendHandle(int s) {
	int charsWritten = 0;
	if(!handle_sent) {
		string msg = code + handle;
		charsWritten = send(s, msg.c_str(), msg.length(), 0);
	}
	return charsWritten;
}

bool Chatter::setNonBlocking(int s) {
	int flags = fcntl(s, F_GETFL, 0);
	if (flags < 0) return false;
	flags = flags&~O_NONBLOCK;
	return (fcntl(s, F_SETFL, flags) == 0) ? true : false;
}

bool Chatter::setTimeout(int s) {
	struct timeval tv;
    tv.tv_sec = TO; 
    tv.tv_usec = TO_MS;
	if (setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0)
		return false;

    if (setsockopt (s, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,  sizeof(tv)) < 0)
		return false;
	return true;
}

void Chatter::clientLoop() {
	setNonBlocking(this->clientSocket);
	setTimeout(this->clientSocket);
	sendHandle(this->clientSocket);
}