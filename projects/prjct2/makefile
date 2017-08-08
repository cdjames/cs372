OPS = -g 
# OPS += -lm
# OPS += -ggdb
OPS += -Wall

OPS_LM = -lm 
OPS_LM += ${OPS} 

PROG = ftpserver

default: all

all: ${PROG}

${PROG}: utils.o ${PROG}.o 
	gcc -o ${PROG}  utils.o ${PROG}.o ${OPS_LM}

${PROG}.o:
	gcc -c ${PROG}.c ${OPS}

utils.o: utils.h 
	gcc -c utils.c  ${OPS}

# encrypt.o: encrypt.c encrypt.h
# 	gcc -g -Wall -c encrypt.c

# newtypes.o: newtypes.c newtypes.h 
# 	gcc -g -Wall -c newtypes.c 

clean:
	rm *.o

cleanall:
	rm ${PROG}
	make clean