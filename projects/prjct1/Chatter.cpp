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

	int amt;
	bool quit = false;
	while(!quit) {
		string msg = "";
		outlock->lock();
		if(!outq->empty()){
			while(!outq->empty()) {
				msg += outq->front();
				outq->pop_front();
			}
			msg += "\n";
			amt = msg.length();
			int success = sendAll(clientSocket, msg.c_str(), msg, &amt);
			if(success == -1) {
				quit = true;
			}
		} else {
			// outlock->unlock();
			// try receiving
			if(!checkAndReceive(clientSocket)) {
				// socket is broken, cleanup and exit
				quit = true;
			}
			
		}
		outlock->unlock();
	}
	_cleanup();
}

void Chatter::_cleanup() {
	inlock->lock();
	inq->push_front(PROC_EXIT);
	inlock->unlock();
}

bool Chatter::checkAndReceive(int s) {
	char chatmsg[MAX_BUF];
	int recvFail;
	fd_set fds; 
	FD_ZERO (&fds);   
    FD_SET (s, &fds);
	/* create timeouts */
    struct timeval tv;
    tv.tv_sec = TO; 
    tv.tv_usec = TO_MS;
    int result = select(s + 1, &fds, NULL, NULL, &tv);
	    // cout << "select returned " << result << endl;
    if(result != -1) { // no error
    	if (FD_ISSET(s, &fds)) { 
    		// try to read data
    		/* receive encrypted text; it will have newline already appended */
			clearString(chatmsg, MAX_BUF+1);
			recvFail = recvMsg(chatmsg, MAX_BUF+1, s);
			// if(recvFail < 0 || recvFail > 0){
			if(recvFail > 0){
				// if(recvFail < 0)
				// 	errorCloseSocket("CLIENT: ERROR reading encrypted text", s);
				// else 
				// if(recvFail > 0)
					errorCloseSocket("Connection closed by server", s);
				return false;
			} else {
				// do your printing
				string cppstring(chatmsg);
				cout << "got a msg: " << cppstring << endl;
				// cout << "got a msg: " << chatmsg << endl;
				// if(!got_aitei_handle && aiteiHandle == "") {
				// 	if(cppstring.find())
				// }
				
			}
    	}
    }
    return true;
}

int Chatter::sendAll(int s, const void * msg, string tosend, int *amountToSend) {
	// figure out how much needs to be sent

	int total = 0; // amount sent
	int amt;
	int bytesToSend = *amountToSend;
	int returnThis = 0;
	/* search for quit */
	if(tosend.find("\\quit") != string::npos)
		return -1;
	// cout << "trying to send" << endl;
	while(total < *amountToSend){
		// send
		amt = send(s, msg+total, bytesToSend, 0);
		/* get out of loop on send error */
		if(amt == -1 || amt == 0){
			if(amt == -1)
				returnThis = amt;
			else
				returnThis = 1;
			break;
		}

		total += amt;
		bytesToSend -= amt;
	}

	// figure out how much was sent and return
	*amountToSend = total;
	
	/* return an error or success depending on result */
	return returnThis;
}

int Chatter::recvMsg(char * buf, int buf_len, int cnctFD){
	/* get size of message */
	int recvFail,
		amtToRecv = buf_len-1;

	memset(buf, '\0', buf_len);

	recvFail = recvAll(cnctFD, buf, &amtToRecv); // Read the client's message from the socket

	if (recvFail < 0) {
		return -1;
	}
	else if (recvFail > 0){
		return 1;
	}
	
	return 0;
}

int Chatter::recvAll(int socketFD, void * buf, int * amountToRecv) {
	int total = 0, // amount received
		amt,
		bytesToRecv = *amountToRecv,
		returnThis = 0;
	
	// cout << "rec. bytes: " << bytesToRecv << endl;
	while(total < *amountToRecv){
		amt = recv(socketFD, buf+total, bytesToRecv, 0);
		/* get out of loop on send error */
		if(amt == -1 || amt == 0){
			if(amt == -1)
				returnThis = amt;
			else
				returnThis = 1;
			break;
		}

		total += amt;
		bytesToRecv -= amt;
	}

	// figure out how much was received and return
	*amountToRecv = total;
	
	/* return an error or success depending on result */
	return returnThis;
}

void Chatter::clearString(char * theString, int size) {
	memset(theString, '\0', size);
}

void Chatter::errorCloseSocket(const char *msg, int socketFD) { 
	cerr << msg << endl;
	close(socketFD);
	// exit(1); 
}