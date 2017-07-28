#ifndef CHATCLIENT_H
#define CHATCLIENT_H

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
// C++ stuff
#include <string>
#include <deque>
#include <iostream>
#include <mutex>

using std::deque;
using std::string;
using std::mutex;
using std::cout;
using std::cin;
using std::endl;

void gatherInput(deque<string> &q);

#endif