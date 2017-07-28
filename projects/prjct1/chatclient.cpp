
/*
select: 
https://stackoverflow.com/questions/9732710/select-stdcin-and-stdgetline-not-playing-well-together#9732927
https://stackoverflow.com/questions/6171132/non-blocking-console-input-c

*/

#include "chatclient.hpp"

/* global variables */
#define PROC_EXIT "owaridayotto"
#define HOST "127.0.0.1"
#define PORT 48834
#define TO 1
#define TO_MS 0

deque<string> out_q;
deque<string> in_q;
mutex inqlock;
mutex outqlock;

int main(int argc, char const *argv[])
{
	thread giThread (gatherInput, &in_q, &out_q);
	// gatherInput(in_q);
	cout << "gatherInput should be working " << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	outqlock.lock();
	if(!out_q.empty()) {
		cout << "found this in the queue: " << out_q.front() << endl;
		out_q.pop_front();
	}
	outqlock.unlock();

	inqlock.lock();
	in_q.push_back(PROC_EXIT);
	inqlock.unlock();
		
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
		    	cin >> i;
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