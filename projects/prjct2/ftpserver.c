/*********************************************************************
** Author: Collin James
** Date: 12/1/16
** Description: Open a listening socket, then do the following:
1. accept connections from client programs (otp_enc)
2. get a text file
3. get a key
4. encrypt the text file using the key
5. send the encrypted file back with newline appended
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>		// for file manipulation
#include <sys/wait.h> 	// for wait/waitpid
#include "utils.h"

const int maxConnections = 5;
const int hdShakeLen = 7;
const int maxBufferLen = 70000;
const char PROG_CODE[] = "otp_enc";
const char PROG_NAME[] = "otp_enc_d";

int main(int argc, char *argv[])
{
	/* Check usage & args */
	if (argc < 2) { 
		fprintf(stderr,"USAGE: %s port\n", argv[0]); 
		exit(1); 
	}

	/* socket variables */
	int listenSocketFD, 
		establishedConnectionFD, 
		portNumber;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in otp_enc_dAddress, clientAddress;
	
	/* keeping track of pids and connections */
	int exitSignal = 0,
		pid = -1, // for storing the child pid
		numConnections = 0, // store number of connections (5 max)
		status,
		wpid;
	struct Pidkeeper thePK = new_PK(pid, exitSignal);

	/* select variables */
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax; // for max fd in select
    
    /* setup select sets by clearing */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

	/*Set up the address struct for this process (the otp_enc_d)*/
	memset((char *)&otp_enc_dAddress, '\0', sizeof(otp_enc_dAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	otp_enc_dAddress.sin_family = AF_INET; // Create a network-capable socket
	otp_enc_dAddress.sin_port = htons(portNumber); // Store the port number
	otp_enc_dAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket and start listen()
	listenSocketFD = setUpSocket(&otp_enc_dAddress, maxConnections);

	/* add listenSocketFD to master set (for select); set the max fd to be the listener */
	FD_SET(listenSocketFD, &master);
	fdmax = listenSocketFD;

	/* loop until exitSignal is received (change to endless loop in final code) 
		In each loop, first collect finished processes, then select an active connection, 
		then process connections */
	int printerr = 0; // for testing
	while(1 && pid != 0) { // run forever (while not a child)
		/* collect finished processes */
		do {
			wpid = waitpid(-1, &status, WNOHANG);
			// printf("wpid=%d\n", wpid);
		} while (wpid > 0);

		/* copy the master set into a temporary set for this iteration */
		read_fds = master;

		if (printerr)
			printf("number of connections = %d\n", numConnections);

		/* do your select and make sure there are no errors */
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            printOutError("otp_enc_d: select failed", 1);
        }
        else { // no errors, look for new connections or data
        	
        	// printf("looking for connections\n");
        	int i;
        	for (i = 0; i <= fdmax; i++)
        	{
        		if(FD_ISSET(i, &read_fds)) { // we have a connection/data to read
        			// printf("new connection\n");
        			/* there is a new listener */
        			if(i == listenSocketFD){
        				sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

        				/* check that you still have available connections and perform handshake */
						if(numConnections < maxConnections) {
							// Accept a connection, blocking if one is not available until one connects
							establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
							
							// 2 Sec Timeout (didn't seem to work)
							struct timeval tv;
							tv.tv_sec  = 2;  
							tv.tv_usec = 0;
							setsockopt( establishedConnectionFD, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
							setsockopt( establishedConnectionFD, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
							
							if (establishedConnectionFD < 0) 
								error("otp_enc_d: ERROR on accept");
							/* if no error, add new connection to your set */
							else {
								numConnections += 1;
								// printf("adding new connection to master\n");
								/* add new connection to master set */
								FD_SET(establishedConnectionFD, &master);

								/* make a new max if necessary */
								if(establishedConnectionFD > fdmax)
									fdmax = establishedConnectionFD;

								/* don't want to accept any more connections if reached max */
								if(numConnections >= maxConnections){
									close(listenSocketFD); // close to reject new connections
									FD_CLR(i, &master); // remove the listening socket from the set; re-add after new client data is read
								}
							}
	        			}
        			}
        			/* we have something to read from a client */
        			else {
	        			// printf("getting data\n");
	        			/* fork a process with doEncrypt... encrypt here */
						thePK = doEncryptInChild(i, PROG_CODE, PROG_NAME, hdShakeLen, 0);
						pid = thePK.pid;
						exitSignal = thePK.status;
						if(pid != 0){
							// printf("exitSignal = %d\n", exitSignal);

							/* remove the connection from the master set and decrement connections */
							FD_CLR(i, &master);
							numConnections -= 1;

							/* check for need to restart socket (don't do in child [pid==0]) */
							if(numConnections < maxConnections){
								/* check whether socket needs to be reopened (don't do in child [pid==0]) */
								if(!FD_ISSET(listenSocketFD, &master)) {
									// printf("otp_enc_d: resetting the connection\n");
									/* re-open the socket */
									listenSocketFD = setUpSocket(&otp_enc_dAddress, maxConnections);
			        				/* add it back to the set and make a new max if necessary */
									FD_SET(listenSocketFD, &master);
									if(listenSocketFD > fdmax)
										fdmax = listenSocketFD;
								}
							}
							wpid = waitpid(pid, &status, WNOHANG);
						}
	        		}
        		} // END FD_ISSET if	
			} // END for loop
		} // END select else
	} // END main while loop

/**** this is done in the parent at the end of the program ****/
	if(pid != 0) {
		close(listenSocketFD); // Close the listening socket
		/* collect finished processes */
		do {
			wpid = waitpid(-1, &status, WNOHANG);
			// printf("wpid=%d\n", wpid);
		} while (wpid > 0);
	}

	return 0; 
}

