
/*
select: 
https://stackoverflow.com/questions/9732710/select-stdcin-and-stdgetline-not-playing-well-together#9732927
https://stackoverflow.com/questions/6171132/non-blocking-console-input-c

Non-block/timeout:
https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking#1549344
https://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections

http://www.cplusplus.com/reference/string/string/getline/

Splitting strings:
https://stackoverflow.com/questions/8448176/split-a-string-into-an-array-in-c
https://stackoverflow.com/questions/11719538/how-to-use-stringstream-to-separate-comma-separated-strings#11719617
*/

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

	cout << "Please enter your chat handle: ";
	getline(cin, handle);
	cout << endl;

	h = HOST;
	p = PORT;
	if (argc >= 2){
		p = std::stoi(argv[1]); // convert port number to int
	}

	/* create chat object and try to connect */
	Chatter ch(p, h, handle, &in_q, &out_q, &inqlock, &outqlock);
	if(!ch.connectToServer()) {
		return 1; // couldn't connect
	}

	/* succeeded in connecting, run input function in its own thread */
	thread giThread (gatherInput, &in_q, &out_q);

	/* start communicating */
	if(ch.is_client) {
		ch.clientLoop(); // loops until '\quit' is received
	}

	/* chat is done, clean up and exit */
	giThread.join(); // close the input thread
	return 0;
}

void gatherInput(deque<string> *inq, deque<string> *outq) {
	string quit_string = "",
			i = "";
	fd_set fds; // set inside loop
	/* create timeouts */
    struct timeval tv;
    tv.tv_sec = TO; 
    tv.tv_usec = TO_MS;
    int result; // store result of select
	do {
		/* need to set these inside the loop; select will modify them*/
		FD_ZERO (&fds);   
	    FD_SET (STDIN_FILENO, &fds);
	    // cout << "checking q" << endl;
	    inqlock.lock();
		if (!inq->empty()) {
			quit_string = inq->front();
			inq->pop_front();
		} else {
			quit_string = "";
		}
		inqlock.unlock();

	    result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
	    // cout << "select returned " << result << endl;
	    if(result != -1) { // no error
	    	if (FD_ISSET(STDIN_FILENO, &fds)) { // check if stdin is ready
		    	getline(cin,i);
			    // cout << "you wrote " << i << endl;
			    outqlock.lock();
			    outq->push_back(i);
			    outqlock.unlock();
			}
		} else {
			cout << "error " << errno << endl;
		}


	} while (quit_string != PROC_EXIT && i != PROC_EXIT); // remove i from this for final implementation
}