OPS = -std=c++11
OPS += -g 
OPS += -ggdb
OPS += -Wall 

CXX = chatclient.cpp
CXX += Chatter.o

PROG1 = chatclient

default: chatclient

Chatter.o:
	g++ -c Chatter.cpp ${OPS}

chatclient: Chatter.o
	g++ ${CXX} -o ${PROG1} ${OPS}

clean:
	rm *.exe *.o