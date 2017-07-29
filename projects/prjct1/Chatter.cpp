/*********************************************************************
** Author: Collin James
** Date: 7/29/17
** Description: Class to implement connecting to, sending to, receiving from server
** for more info see "Chatter.hpp"
*********************************************************************/

#include "Chatter.hpp"

Chatter::Chatter(int port, string host, string handle, deque<string> *inq, deque<string> *outq,
		mutex *inlock, mutex *outlock) {
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
		cerr << "...ERROR, no such host: " << this->host << endl;
		return false;
	}
	// Copy in the address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	// Set up the socket
	clientSocket = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (clientSocket < 0) { 
		cerr << "...ERROR opening socket" << endl; 
		return false; 
	}

	setNonBlocking(this->clientSocket);
	setTimeout(this->clientSocket);

	// Connect to server with timeout
	int connected = _connect(clientSocket, TO_CONNECT, serverAddress);
	// yay, you are connected!
	return connected;
}

bool Chatter::_connect(int s, int to, struct sockaddr_in serverAddress) {
	bool success = false;
	int connected = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (connected < 0) { // can't immediately connect socket to address
		 // cerr << "...ERROR connecting to " << this->host << ":" << this->port << endl; 
		 // return false;

		/* see if socket is ready to connect to (ready to write to) */
		fd_set writeSet; 
		FD_ZERO (&writeSet);   
		FD_SET (clientSocket, &writeSet);
		/* create timeouts */
		struct timeval tv;
		tv.tv_sec = to; 
		tv.tv_usec = TO_MS;
		
		int result = select(clientSocket+1, NULL, &writeSet, NULL, &tv);
		if(result == 1) { // no select error
			int so_error;
	        socklen_t len = sizeof so_error;

	        getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, &so_error, &len);

	        if (so_error == 0) // connected
	            success = true;
		} 
	} else if (connected == 0) {
		success = true;
	}
	
	if (success) {
		cout << "...Connected to " << this->host << ":" << this->port << endl;
		is_client = true;
	} else {
		cout << "...Connection to " << this->host << ":" << this->port << " timed out" << endl;
	}

	return success;
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
	flags = flags | O_NONBLOCK;
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
	// setNonBlocking(this->clientSocket);
	// setTimeout(this->clientSocket);
	sendHandle(this->clientSocket);

	int amt;
	bool quit = false;
	while(!quit) {
		string msg = "";
		outlock->lock();
		if(!outq->empty()){
			while(!outq->empty() && msg.length() < MAX_BUF) {
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
	cout << "...quitting" << endl;
	inlock->lock();
	inq->push_front(PROC_EXIT);
	inlock->unlock();
}

bool Chatter::checkAndReceive(int s) {
	char chatmsg[MAX_BUF]; // hold received message
	int recvFail; // return message from recvMsg()
	/* set up select() variables */
	fd_set fds; 
	FD_ZERO (&fds);   
    FD_SET (s, &fds);
	/* create timeouts */
    struct timeval tv;
    tv.tv_sec = TO; 
    tv.tv_usec = TO_MS;
    /* see if socket is ready to read on */
    int result = select(s + 1, &fds, NULL, NULL, &tv);

    if(result != -1) { // no error
    	if (FD_ISSET(s, &fds)) { // ready to read
    		/* receive text */
			recvFail = recvMsg(chatmsg, MAX_BUF+1, s);
			if(recvFail > 0){
				errorCloseSocket("...Connection closed by server", s);
				return false;
			} else {
				// do your printing
				string cppstring(chatmsg); // create a c++ string from the c string

				/* determine who you're speaking to */
				if(!got_aitei_handle && aiteiHandle == "") {
					/* try to find the code */
					std::size_t pos = cppstring.find(this->code);
					if(pos != string::npos) {
						/* extract info after ":" */
						pos = cppstring.find(":");
						this->aiteiHandle = cppstring.substr(pos+1, string::npos);
						this->got_aitei_handle = true;
						// print
						cout << "...Speaking with " << aiteiHandle << endl;
					}
				} else { // print messages normally
					/* just in case messages are coming in really fast, make sure they don't
					   have any \n mixed in by splitting on \n */
					vector<string> strparts;
					splitString(cppstring, strparts);
					for (int i = 0; i < strparts.size(); i++)
						cout << aiteiHandle << "> " << strparts[i] << endl;
				}
				
			}
    	}
    }
    return true; // all good!
}

int Chatter::sendAll(int s, const void * msg, string tosend, int *amountToSend) {
	int total = 0; // amount sent
	int amt;
	int bytesToSend = *amountToSend;
	int returnThis = 0;
	/* search for quit and get out here! */
	if(tosend.find("\\quit") != string::npos)
		return -1;
	/* otherwise, send everything */
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

	clearString(buf, buf_len);

	recvFail = recvAll(cnctFD, buf, &amtToRecv); // Read the client's message from the socket

	if (recvFail < 0) { // connection closed
		return -1;
	}
	else if (recvFail > 0) { // no data sent
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
			if(amt == -1) // error, connection closed
				returnThis = amt;
			else
				returnThis = 1; // no data sent
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

void Chatter::splitString(string str, vector<string> &container) {
	std::istringstream ss(str);
	string part;
 
    while(std::getline(ss, part, '\n')) {
    	container.push_back(part);
    }
}