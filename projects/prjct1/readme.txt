CS 372 Project 1 Readme
Collin James

Building:
type 'make' to make the C++ chat client

Running: 
	Server:
	`python chatserve.py`    					(default port 48834, default IP localhost) 
	`python chatserve.py 192.168.1.12 40404`    (custom port & IP) 
	`python chatserve.py -h` 					(for help) 

	Client (C++):
	`./chatclient`    					(default port 48834, default IP localhost)
	`./chatclient 192.168.1.12 40404 `  (custom port 48834 & IP)
	`./chatclient -h` 					(for help)

Usage:
	1) Start the server first, then the client 
	2) Enter your handle and press enter
	3) Start typing and send whenever you would like
	4) Both client and server end the conversation by typing '\quit'
	5) The server will continue to run and accept new clients until you press Ctrl-C

Extra Credit:
	1) The Python app is multi-process (the C++ was multi-threaded and it worked GREAT on OS X and Cygwin, but didn't work on flip :( I left the code in chatclient.cpp so you can see my implementation)
	2) chatserve.py is also a chat client; in this case, whoever starts chatserve.py first is the server
	3) Either host can send at any time