/*********************************************************************
** Author: Collin James
** Date: 7/29/17
** Description: CS 372 Project 1: Main function for chat client
** Uses ideas and minimal code from the following:

select/Non-block/timeout:
https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking#1549344
https://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections
http://unix.derkeiler.com/Newsgroups/comp.unix.programmer/2008-06/msg00108.html
https://stackoverflow.com/questions/2597608/c-socket-connection-timeout#2597774
https://stackoverflow.com/questions/9732710/select-stdcin-and-stdgetline-not-playing-well-together#9732927
https://stackoverflow.com/questions/6171132/non-blocking-console-input-c
https://stackoverflow.com/questions/2597608/c-socket-connection-timeout#2597774

input:
http://www.cplusplus.com/reference/string/string/getline/

Splitting strings:
https://stackoverflow.com/questions/8448176/split-a-string-into-an-array-in-c
https://stackoverflow.com/questions/11719538/how-to-use-stringstream-to-separate-comma-separated-strings#11719617

*********************************************************************/

#include "chatclient.hpp"

/* global variables, used by main thread (via Chatter object)
   and input thread for communication */
deque<string> out_q;
deque<string> in_q;
mutex inqlock;
mutex outqlock;

int main(int argc, char const *argv[])
{
	string handle,
		   h;
	int p;

	/* set default host and port */
	h = HOST;
	p = PORT;

	/* deal with command line parameters */
	if (argc >= 2){
		if(argv[1][0] == '-' && argv[1][1] == 'h') { // print usage
			cout << USAGE << endl;
			exit(0);
		} else { // get host IP
			h = argv[1];
			cout << "h=" << h << endl;
		}
		if (argc >= 3) { // get port number
			p = std::stoi(argv[2]); // convert port number to int
		}
	}

	cout << "Please enter your chat handle: ";
	getline(cin, handle);
	cout << endl;


	/* create chat object and try to connect */
	Chatter ch(p, h, handle, &in_q, &out_q, &inqlock, &outqlock);
	if(!ch.connectToServer()) {
		return 1; // couldn't connect
	}

	/* succeeded in connecting, run input function in its own thread */
	// thread giThread (gatherInput, &in_q, &out_q);

	/* start communicating */
	if(ch.is_client) {
		ch.clientLoop(); // loops until '\quit' is received
	}

	/* chat is done, clean up and exit */
	// giThread.join(); // close the input thread
	return 0;
}

/*********************************************************************
** Description: gatherInput()
** Params: deque inq for ending the loop; outq for communicating messages to main thread
** Waits for user to input text on stdin and relays that to main thread
*********************************************************************/
// void gatherInput(deque<string> *inq, deque<string> *outq) {
// 	string quit_string = "", // special "code" that will be sent from main to end loop
// 			i = ""; // input from user 
// 	fd_set fds; // for working with select(); set inside loop

// 	/* create timeouts from global defaults */
//     struct timeval tv;
//     tv.tv_sec = TO; 
//     tv.tv_usec = TO_MS;
//     int result; // store result of select

//     /* loop until you get the quit signal. Relay messages to the outq */
// 	do {
// 		/* need to set these inside the loop; select will modify them*/
// 		FD_ZERO (&fds);   
// 	    FD_SET (STDIN_FILENO, &fds); // we want to read from stdin
// 	    /* make sure no one else is trying to use the queue and gather quit signal if any */
// 	    inqlock.lock();
// 		if (!inq->empty()) {
// 			quit_string = inq->front();
// 			inq->pop_front();
// 		} else {
// 			quit_string = "";
// 		}
// 		inqlock.unlock();
// 		/* figure out if we can read on stdin */
// 	    result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
// 	    if(result != -1) { // no error
// 	    	if (FD_ISSET(STDIN_FILENO, &fds)) { // check if stdin is ready
// 		    	getline(cin,i);
// 		    	/* put the user's message in the outq for the Chatter object */
// 			    outqlock.lock();
// 			    outq->push_back(i);
// 			    outqlock.unlock();
// 			}
// 		} else {
// 			cout << "error " << errno << endl;
// 		}
// 		/* you can input the code to kill the thread (i) 
// 			It's in Japanese so people most likely won't stumble on it by accident 
// 			Normally, the main thread will send this code */
// 	} while (quit_string != PROC_EXIT && i != PROC_EXIT); 
// }