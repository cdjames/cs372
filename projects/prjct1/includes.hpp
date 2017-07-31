/*********************************************************************
** Author: Collin James
** Date: 7/29/17
** Description: Stuff needed by Chatter and chatclient (main)
*********************************************************************/

#ifndef INCLUDES_H
#define INCLUDES_H

// C stuff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#include <cstring>
#include <fcntl.h> // enable non-blocking socket
// C++ stuff
#include <string>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <sstream>  //for std::istringstream
// for my convenience
using std::deque;
using std::string;
using std::mutex;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::thread;
using std::vector;

/* global variables */
#define USAGE "./chatclient [-help] [ip] [port]"
#define PROC_EXIT "owaridayotto" // "code word" to tell a thread to stop
#define HOST "127.0.0.1" // default IP
#define PORT 48834 // default port
#define TO 0 // default timout in seconds
#define TO_CONNECT 3 // connection timeout in seconds
#define TO_MS 500000 // 500 ms, default timout in ms
#define MAX_BUF 70000 // maximum length of message

#endif