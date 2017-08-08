/*********************************************************************
** Author: Collin James
** Date: 12/1/16
** Description: Utility functions used by otp_* and keygen. See more 
** info about functions in utils.h
*********************************************************************/
#include "utils.h"

const char PROG_NAME[] = "ftpserver";

struct Pidkeeper new_PK(pid_t pid, int status)
{
	struct Pidkeeper pk;
	pk.pid = pid;
	pk.status = status;
	return pk;
}

int sendAll(int socketFD, void * msg, int * amountToSend) {
	// figure out how much needs to be sent

	int total = 0; // amount sent
	int amt;
	int bytesToSend = *amountToSend;
	int returnThis = 0;
	
	while(total < *amountToSend){
		// send
		amt = send(socketFD, msg+total, bytesToSend, 0);
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

int sendMsg(char * text, int cnctFD){
	/* get size of message */
	int sizeOfString = strlen(text),
		sendFail,
		amtToSend = sizeof(sizeOfString);

	sendFail = sendAll(cnctFD, &sizeOfString, &amtToSend); // Read int from the socket
	if (sendFail < 0) {
		// printOutError("ERROR reading from socket", 1);
		return -1;
	}
	else if (sendFail > 0) {
		// printOutError("Socket was closed", 1);
		return 1;
	}
	if (amtToSend < sizeof(sizeOfString)) 
		printOut("WARNING: Not all data written to socket!", 1);
	// printf("CLIENT: sent size\n");
	/* send the plaintext message */
	amtToSend = sizeOfString;
	sendFail = sendAll(cnctFD, text, &amtToSend); // Read the client's message from the socket
	if (sendFail < 0) {
		// printOutError("ERROR writing to socket", 1);
		return -1;
	}
	else if (sendFail > 0) {
		// printOutError("Socket was closed", 1);
		return 1;
	}
	if (amtToSend < sizeOfString) 
		printOut("WARNING: Not all data written to socket!", 1);
	
	// printf("SERVER: I received this from the client: \"%s\"\n", text);

	return 0;
}

int recvAll(int socketFD, void * buf, int * amountToRecv) {
	// figure out how much needs to be sent
	printf("in recvAll=%d\n", *amountToRecv);
	int total = 0; // amount received
	int amt;
	int bytesToRecv = *amountToRecv;
	int returnThis = 0;
	
	while(total < *amountToRecv){
		amt = recv(socketFD, buf+total, bytesToRecv, 0);
		printf("amt=%d\n", amt);
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

	// figure out how much was sent and return
	*amountToRecv = total;
	
	/* return an error or success depending on result */
	return returnThis;
}

int recvMsg(char * buf, int buf_len, int cnctFD){
	/* get size of message */
	int sizeOfString = 0,
		recvFail,
		amtToRecv = sizeof(sizeOfString);

	recvFail = recvAll(cnctFD, &sizeOfString, &amtToRecv); // Read int from the socket
	if (recvFail < 0) {
		// printOutError("ERROR reading from socket", 1);
		return -1;
	} 
	else if (recvFail > 0){
		// printOutError("Socket closed", 1);
		return 1;
	}

	if (amtToRecv < sizeof(sizeOfString)) 
		printOut("WARNING: Not all data read from socket!", 1);
	// printf("SERVER: received size\n");
	/* read the plaintext message */
	memset(buf, '\0', buf_len);
	amtToRecv = sizeOfString;
	recvFail = recvAll(cnctFD, buf, &amtToRecv); // Read the client's message from the socket
	if (recvFail < 0) {
		// printOutError("ERROR reading from socket", 1);
		return -1;
	}
	else if (recvFail > 0){
		// printOutError("Socket closed", 1);
		return 1;
	}
	if (amtToRecv < sizeOfString) 
		printOut("WARNING: Not all data read from socket!", 1);
	
	// printf("SERVER: I received this from the client: \"%s\"\n", buf);

	return 0;
}

/*********************************************************************
** Description: 
** Automates memset() because 'memset' isn't very descriptive
*********************************************************************/
void clearString(char * theString, int size) {
	memset(theString, '\0', size);
}

void printOutError(const char * outString, int newln){
	fputs(outString, stderr);
	fflush(stderr); // flush the print buffer
	if(newln){	
		fputs("\n", stderr);
		fflush(stderr);
	}
}

void printOut(char * outString, int newln){
	fputs(outString, stdout);
	fflush(stdout); // flush the print buffer
	if(newln){	
		fputs("\n", stdout);
		fflush(stdout);
	}
}

void error(const char *msg) { 
	// fprintf(stderr, "%s\n", msg);
	printOutError(msg, 1);
	// perror(msg); 
	exit(1); 
} // Error function used for reporting issues

void error2(const char *msg) { 
	// fprintf(stderr, "%s\n", msg);
	printOutError(msg, 1);
	// perror(msg); 
	exit(2); 
} // Error function used for reporting issues

void errorCloseSocket(const char *msg, int socketFD) { 
	// fprintf(stderr, "%s\n", msg);
	printOutError(msg, 1);
	// perror(msg); 
	close(socketFD);
	exit(1); 
} // Error function used for reporting issues

void errorCloseSocketNoExit(const char *msg, int socketFD) { 
	// perror(msg); 
	printOutError(msg, 1);
	close(socketFD);
} // Error function used for reporting issues

/*********************************************************************
** Description: 
** Return a random number
**
** Ex: min = 3, max = 9, rand % (7) = 0...6 + 3 = 3...9
*********************************************************************/
int getRandom(int min, int max) {
	return (rand() % (max+1-min) + min);
}

int hasValidChars(char * text){
	int textlen = strlen(text),
		i;

	for (i = 0; text[i] != '\0' || i < textlen; i++)
	{
		// printf("char is %d\n", text[i]);
		if( text[i] != ' ' && (text[i] < 'A' || text[i] > 'Z') )
			return 0;
	}

	/* all characters checked out! */
	return 1;
}

void checkText(char * text, int socketFD, char * fname) {
	int okay = hasValidChars(text);
	if(!okay){
		printOutError("opt_enc: ", 0);
		printOutError(fname, 0);
		errorCloseSocket(" contains bad characters", socketFD);
	}
}

int checkFile(char * file){
	/* create directory, checking for existence first */
	struct stat checkfor;
	if (stat(file, &checkfor) == -1) {
		return 0;
	}

	return 1;
}

/* opt_enc_d and opt_dec_d specific files */
int setUpSocket(struct sockaddr_in * serverAddress, int maxConn){
	// Set up the socket
	int yes = 1;
	int listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) 
		error("SERVER: ERROR opening socket");

	// reuse previously used ports before they are released by OS (from beej.us) -- doesn't work
    setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress)) < 0) // Connect socket to port
		error("SERVER: ERROR on binding");
	
	int listening;
	listening = listen(listenSocketFD, maxConn); // Flip the socket on - it can now receive up to 5 connections
	if(listening < 0)
		error("SERVER: failed to listen");

	return listenSocketFD;
}

int makeConnection(int portNumber) {
	int socketFD;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { 
		fprintf(stderr, "%s: ERROR, no such host\n", PROG_NAME); 
		return -1; 
	}
	memcpy((char*)serverHostInfo->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) {
		fprintf(stderr, "%s: ERROR opening socket\n", PROG_NAME);
		return -1;
	}
	
	// Connect to server
	int connected = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	// printf("otp_enc: connected=%d\n", connected);
	if (connected < 0) { // Connect socket to address
		fprintf(stderr, "%s: ERROR connecting\n", PROG_NAME);
		return -1;
	}
	
	return socketFD;
}

int makeDataConnection(int clientFD) {
	/* send code 1 to let client know it should send a port */
	int request = 1,
		amtToSend = sizeof(request),
		dataport = 65534,
		amtToRecv = sizeof(dataport);
		/* send a request for the port number */
	// printf("send %d bytes\n", amtToSend);
	char code[] = "1";
	printf("send %d bytes\n", strlen(code));
	// int sendFail = sendAll(clientFD, code, strlen(code)+1);
	int sendFail = send(clientFD, code, strlen(code), 0);
	// int sendFail = sendAll(clientFD, &request, &amtToSend);

	/* receive port number */
	int recvFail = recvAll(clientFD, &dataport, &amtToRecv); // Read the client's message from the socket
	if (recvFail < 0) {
		return -1;
	}
	else if (recvFail > 0){
		return -2;
	}

	/* make a data connection */
	int dataFD = makeConnection(dataport);
	if(dataFD < 0) {
		return -3;
	}

	return dataFD;
}

int readFile(char * fname, char * buffer) {
	FILE * file;

	/* open the file and check for errors */
	file = fopen(fname,"r");

	if (file == NULL)
		return -1; // could not open file

	/* get plaintext */
	clearString(buffer, MAX_BUF_LEN+1);
	if(fgets(buffer, MAX_BUF_LEN, file) == NULL)
		return -2; // could not read file

	fclose(file);

	return 0;
}

int readDirectory(char * path, char * buffer) {
	/* help here: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c#612176 
		and here: https://stackoverflow.com/questions/11531245/reading-directories-using-readdir-r#27164624
	*/
	DIR * dirname = opendir(path);
	if(dirname == NULL) // open error
		return 1;

	struct dirent *dir_entry = NULL; // for storing directory entry
	int first_run = 1;
	int len = 0; // track how many bytes you have read; keep this under the max buffer length
	clearString(buffer, strlen(buffer));
	int bef_errno = errno;
	while( ( dir_entry = readdir(dirname) ) != NULL && len < MAX_BUF_LEN) {
		if(strcmp(dir_entry->d_name, ".") != 0 && strcmp(dir_entry->d_name, ".") != 0) { // don't append . and ..
			/* append the string to the end of the buffer */
			if(first_run) {
				strcpy(buffer, dir_entry->d_name);
			}
			else {
				strcat(buffer, dir_entry->d_name);
			}
			strcat(buffer, "\n");
			len += strlen(dir_entry->d_name)+1;
		}
	}
	if(dir_entry == NULL && bef_errno != errno) // there was a read error as errno has been changed
		return 1;

	return 0;
}

struct Pidkeeper sendFileInChild(int clientFD) {
	// Get the message from the client and process it
	/* set up pipe for communicating failures with parent */

	int r, 
		pipeFDs[2],
		exitSignal = 0, // send this data to parent
		stat_msg,	// receive data here
		pipe_status; // save status of pipe
	long int msg_size = sizeof(exitSignal);

	if( (pipe_status = pipe(pipeFDs)) == -1) {
		perror("failed to set up pipe");
		return new_PK(-1, -1); // send -1 to represent no pipe
	}

	/* fork a process */
	int pid = fork(),
		status;

	/* in child, process commands & send data */
	if(pid == 0) {
		#ifdef DEBUG
		fprintf(stdout, "client connected\n");
		#endif
		int exitSignal = 0,
			recvFail,
			sendFail,
			amtToRecv = 2,
			request = 0,
			amtToSend = sizeof(request),
			dataFD;
		char cmd[3],
			 fname[NAME_MAX-1],
			 bad_cmd_msg [] = "1:commands are -l or -g",
			 ferror_msg [] = "2:file not found",
			 direrror_msg [] = "3:could not read directory",
			 f_contents[MAX_BUF_LEN+1];
		clearString(cmd, 3);

		/* receive a command */
		// recvFail = recvAll(clientFD, cmd, 512); // Read the client's message from the socket
		recvFail = recv(clientFD, cmd, 2, 0); // Read the client's message from the socket
		#ifdef DEBUG
		fprintf(stdout, "got this from client: %s, %d\n", cmd, recvFail);
		printf("%d", recvFail);
		#endif

		if (recvFail < 0) {
			printOutError(PROG_NAME, 0);
			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
			errorCloseSocketNoExit(": ERROR reading from socket", clientFD);
			return new_PK(pid, -1);
		}
		else if (recvFail == 0){
			errorCloseSocketNoExit("SERVER: Socket closed by client", clientFD);
			close(clientFD);
			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
			return new_PK(pid, -1);
		}

		/* got a command, check that it's valid */
		if(cmd[1] == 'l') {
			/* send code 1 to let client know it should send a port; get port #; make data connection */
			dataFD = makeDataConnection(clientFD);
			if (dataFD == -1) {
				printOutError(PROG_NAME, 0);
				sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
				errorCloseSocketNoExit(": ERROR reading from socket", clientFD);
				return new_PK(pid, -1);
			} else if (dataFD == -2){
				// errorCloseSocketNoExit("SERVER: Socket closed by client", cnctFD);
				close(clientFD);
				sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
				return new_PK(pid, -1);
			} else if(dataFD < -3) {
				printOutError(PROG_NAME, 0);
				sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
				errorCloseSocketNoExit(": ERROR reading from socket", dataFD);
				close(clientFD);
				return new_PK(pid, -1);
			}

			/* compile directory into f_contents */
			int read_fail = readDirectory(".", f_contents);
			if(read_fail) {
				sendFail = sendAll(clientFD, direrror_msg, strlen(direrror_msg));
				printOutError(PROG_NAME, 0);
				sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
				errorCloseSocketNoExit(": couldn't read directory", dataFD);
				close(clientFD);
				return new_PK(pid, -1);
			}

			/* send the directory */
			sendFail = sendAll(dataFD, f_contents, strlen(f_contents));

			/* close the connection */
			close(dataFD);

		} else if (cmd[0] == 'g') {
			/* ask for file name by sending a "command" of 2 */
			request = 2;
			sendFail = sendAll(clientFD, &request, &amtToSend);

			/* receive file name */
			clearString(fname, NAME_MAX);
			recvFail = recvMsg(fname, NAME_MAX, clientFD);
			if (recvFail < 0) {
				printOutError(PROG_NAME, 0);
				sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
				errorCloseSocketNoExit(": ERROR reading from socket", clientFD);
				return new_PK(pid, -1);
			}
			else if (recvFail > 0){
				// errorCloseSocketNoExit("SERVER: Socket closed by client", clientFD);
				close(clientFD);
				sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
				return new_PK(pid, -1);
			}

			/* check for existence of file  */
			int found = checkFile(fname);
			if(!found) {
				/* send error message to client */
				request = 3;
				sendFail = sendAll(clientFD, &request, &amtToSend);

				sendFail = sendAll(clientFD, ferror_msg, strlen(ferror_msg));
				sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
				errorCloseSocketNoExit(": no such file", clientFD);
				return new_PK(pid, -1);
			}

			/* read the file */
			int read_err = readFile(fname, f_contents);
			if(read_err >= 0) { // success, try to make data connection
				/* make data connection */
				dataFD = makeDataConnection(clientFD);
				if (dataFD == -1) {
					printOutError(PROG_NAME, 0);
					sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
					errorCloseSocketNoExit(": ERROR reading from socket", clientFD);
					return new_PK(pid, -1);
				} else if (dataFD == -2){
					// errorCloseSocketNoExit("SERVER: Socket closed by client", cnctFD);
					close(clientFD);
					sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
					return new_PK(pid, -1);
				} else if(dataFD < -3) {
					printOutError(PROG_NAME, 0);
					sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
					errorCloseSocketNoExit(": ERROR reading from socket", dataFD);
					close(clientFD);
					return new_PK(pid, -1);
				}

				/* send the file on the data connection */
				sendFail = sendAll(dataFD, f_contents, strlen(f_contents));

				/* close the connection */
				close(dataFD);
			}
		} else {
			/* ask for file name by sending a "command" of 2 */
			request = 3;
			sendFail = sendAll(clientFD, &request, &amtToSend);

			/* bad command, send error message */
			sendFail = sendAll(clientFD, bad_cmd_msg, strlen(bad_cmd_msg));
			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
			errorCloseSocketNoExit(": bad command", clientFD);
			return new_PK(pid, -1);
		}

		close(clientFD);
		/* send error status message to parent, i.e. 1 (sending int disguised as void *) 
		you will never get to this point if exec occurs, and output pipe will be closed
		on exec, causing read to receive 0 */
		if(pipe_status != -1 && exitSignal == 1)	
			write(pipeFDs[1], &exitSignal, msg_size);
	} 
	/* let the parent wait to collect, but don't hang */
	else if (pid > 0) {
		if(pipe_status != -1)
			close(pipeFDs[1]); // close output pipe

		pid_t exitpid;
		exitpid = waitpid(pid, &status, WNOHANG);
		/* read the message from the child if there is one */
		if(pipe_status != -1){
			r = read(pipeFDs[0], &stat_msg, msg_size);
			if (r > 0)
				exitSignal = stat_msg;
		}
	}

	return new_PK(pid, exitSignal);

}

// struct Pidkeeper doEncryptInChild(int cnctFD, const char * PROG_CODE, const char * PROG_NAME, const int hdShakeLen, int dec) {
// 	// Get the message from the client and process it
// 	/* set up pipe for communicating failures with parent */
// 	const int maxBufferLen = 70000;

// 	int r, 
// 		pipeFDs[2],
// 		exitSignal = 0, // send this data to parent
// 		stat_msg,	// receive data here
// 		pipe_status; // save status of pipe
// 	long int msg_size = sizeof(exitSignal);

// 	if( (pipe_status = pipe(pipeFDs)) == -1)
// 		perror("failed to set up pipe");

// 	/* fork a process */
// 	int pid = fork(),
// 		status;
// 	// printf("connection ID is %d\n", cnctFD);

// 	/* in child, do the encryption */
// 	if(pid == 0) {
// 		if(pipe_status != -1){
// 			close(pipeFDs[0]); // close input pipe
// 			// fcntl(pipeFDs[1], F_SETFD, FD_CLOEXEC); // close output pipe on exec (NOT doing exec!)
// 		}

// 		char hdShakeBuffer[hdShakeLen+1];
// 		char ptBuffer[maxBufferLen+1];
// 		char keyBuffer[maxBufferLen+1];
// 		char encryptText[maxBufferLen+1];
// 		int exitSignal = 0,
// 			recvFail,
// 			sendFail,
// 			amtToRecv;

// 		/* read initial handshake message (opt_enc) */
// 		memset(hdShakeBuffer, '\0', hdShakeLen+1);
// 		amtToRecv = hdShakeLen;
// 		// printf("SERVER: receiving handshake\n");
// 		recvFail = recvAll(cnctFD, hdShakeBuffer, &amtToRecv); // Read the client's message from the socket
// 		if (recvFail < 0) {
// 			printOutError(PROG_NAME, 0);
// 			errorCloseSocketNoExit(": ERROR reading from socket", cnctFD);
// 			return new_PK(pid, -1);
// 		}
// 		else if (recvFail > 0){
// 			// errorCloseSocketNoExit("SERVER: Socket closed by client", cnctFD);
// 			close(cnctFD);
// 			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
// 			return new_PK(pid, -1);
// 		}
// 		// printf("SERVER: handshake = %s\n", hdShakeBuffer);

// 		/* determine if correct program is connecting */
// 		int accepted = 0,
// 			amtToSend = sizeof(accepted);
		
// 		if(strcmp(hdShakeBuffer, PROG_CODE) == 0)
// 			accepted = 1; // accept the server

// 		/* send a code accepting or denying the connection */
// 		sendFail = sendAll(cnctFD, &accepted, &amtToSend);
// 		if (!accepted) {
// 			// printOutError(PROG_NAME, 0);
// 			// errorCloseSocketNoExit(": Unrecognized connecting program", cnctFD);
// 			close(cnctFD); // error in client
// 			return new_PK(pid, -1);
// 		}
		
// 		/* correct program connected; read the plaintext message*/
// 		memset(ptBuffer, '\0', maxBufferLen+1);
// 		recvFail = recvMsg(ptBuffer, maxBufferLen+1, cnctFD);
// 		if (recvFail < 0) {
// 			printOutError(PROG_NAME, 0);
// 			errorCloseSocketNoExit(": ERROR reading from socket", cnctFD);
// 			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
// 			return new_PK(pid, -1);
// 		}
// 		else if (recvFail > 0){
// 			// errorCloseSocketNoExit("SERVER: Socket closed by client", cnctFD);
// 			close(cnctFD);
// 			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
// 			return new_PK(pid, -1);
// 		}

// 		/* read the key */
// 		memset(keyBuffer, '\0', maxBufferLen+1);
// 		recvFail = recvMsg(keyBuffer, maxBufferLen+1, cnctFD);
// 		if (recvFail < 0) {
// 			// errorCloseSocketNoExit("SERVER: ERROR reading from socket", cnctFD);
// 			close(cnctFD);
// 			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
// 			return new_PK(pid, -1);
// 		}
// 		else if (recvFail > 0){
// 			// errorCloseSocketNoExit("SERVER: Socket closed by client", cnctFD);
// 			close(cnctFD);
// 			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
// 			return new_PK(pid, -1);
// 		}
// 		// printf("SERVER: read: %s\n", keyBuffer);

// 		/* do the encryption (dec=0) or decryption(dec=1) */
// 		memset(encryptText, '\0', maxBufferLen+1);
		
// 		encrypt(ptBuffer, keyBuffer, encryptText, dec);
// 		/* add \n to back of encrypted text */
// 		encryptText[strlen(encryptText)] = '\n';
// 		// printf("encrypted text = %s\n", encryptText);

// 		/* send the encrypted text back to the client */
// 		sendFail = sendMsg(encryptText, cnctFD); // Write to the server
// 		if (sendFail < 0) {
// 			printOutError(PROG_NAME, 0);
// 			sendErrorToParent(pipe_status, pipeFDs[1], msg_size);
// 			errorCloseSocketNoExit(": ERROR writing encrypted text to socket", cnctFD);
// 			return new_PK(pid, -1);
// 		}

// 		/* Close the existing socket which is connected to the client */
// 		close(cnctFD); // 

// 		/* send error status message of 0 (no error) to parent */
// 		if(pipe_status != -1){
// 			exitSignal = 0;
// 			write(pipeFDs[1], &exitSignal, msg_size);
// 			close(pipeFDs[1]);
// 		}
// 	} 
// 	/* let the parent wait to collect, but don't hang */
// 	else if (pid > 0) {
// 		if(pipe_status != -1)
// 			close(pipeFDs[1]); // close output pipe

// 		pid_t exitpid;
// 		exitpid = waitpid(pid, &status, WNOHANG);
// 		/* read the message from the child if there is one */
// 		if(pipe_status != -1){
// 			r = read(pipeFDs[0], &stat_msg, msg_size);
// 			if (r > 0)
// 				exitSignal = stat_msg;
// 		}
// 	}

// 	return new_PK(pid, exitSignal);
// }

void sendErrorToParent(int pipe_status, int pipeFD, long int msg_size){
	if(pipe_status != -1){
		int exitSignal = 1;	
		write(pipeFD, &exitSignal, msg_size);
		close(pipeFD);
	}
}


int mod (int x, int y)
{
   int themod = x % y;
   if(themod < 0)
     themod += y;
   return themod;
}

// void encrypt(char * instring, char * keystring, char * outstring, int dec){
// 	const int ASCII_DIF = 65;
// 	const int SPACE_VAL = 32;
// 	const int TOP_VAL = 27;
// 	/* vars for plaintext char, key char, intermediate char, encrypted char */
// 	int pc, kc, ic, ec, 
// 		i = 0;

// 	/* until end of string */
// 	while (instring[i] != '\0') {
// 		/* convert characters A-Z \s into 0-26 for in and key */
// 		pc = instring[i] - ASCII_DIF;
// 		if(pc < 0) // must be space since it is 32 - 65
// 			pc = TOP_VAL-1;
// 		// printf("pc = %d\n", pc);
// 		kc = keystring[i] - ASCII_DIF;
// 		if(kc < 0)
// 			kc = TOP_VAL-1;
// 		/* get an intermediate value for clarity (encrypt or decrypt) */
// 		ic = (!dec) ? pc + kc : pc - kc;
		
// 		/* your encrypted int will be modulo of ic if less than top*/
// 		if(!dec)
// 			ec = (ic >= TOP_VAL) ? ic-TOP_VAL : mod(ic, TOP_VAL);
// 		/* your decrypted int will be modulo of ic if greater than zero*/
// 		else
// 			ec = (ic < 0) ? ic+TOP_VAL : mod(ic, TOP_VAL);

// 		if(ec == TOP_VAL-1)
// 			ec = SPACE_VAL - ASCII_DIF;

// 		outstring[i] = ec + ASCII_DIF;
// 		i += 1;
// 	}
// }