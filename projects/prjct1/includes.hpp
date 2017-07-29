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

using std::deque;
using std::string;
using std::mutex;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::thread;

/* global variables */
#define PROC_EXIT "owaridayotto"
#define HOST "127.0.0.1"
#define PORT 48834
#define TO 1
#define TO_MS 0
#define MAX_BUF 70000

#endif