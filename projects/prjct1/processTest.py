# https://stackoverflow.com/questions/8976962/is-there-any-way-to-pass-stdin-as-an-argument-to-another-process-in-python#8981813
# https://docs.python.org/2/library/socket.html

from multiprocessing import Process, Queue, Lock
# from threading import Thread, Lock
# from Queue import Queue
import time
import sys
from select import select
import os
import socket
import signal



out_q = Queue()
in_q = Queue()
mutex = Lock()
timeout = 1
host = "127.0.0.1"
port = 48834
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
is_client = False
is_server = False
original_sigint = signal.getsignal(signal.SIGINT)

def gatherInput(std_in, q):
	''' this will read the input for both client and server, put any received input in 
		a "buffer (out_q). It will only close when it receives the password'''
	try:
		i = q.get(False) # passing False causes an exception to be thrown upon an empty queue
	except Exception, e:
		i = ""

	while i != "owaridayotto":
		mutex.acquire() # not sure if I need to use mutex here, but I'm going to to be safe
		rlist, _, _ = select([std_in], [], [], timeout)
		if rlist:
			i = std_in.readline()
			# print i
			out_q.put(i)
		# print std_in.readline()
		mutex.release()
		try:
			i = q.get(False)
		except Exception, e:
			i = ""

def clientLoop(q):
	''' this will look for something in the out_q and send to a conversant '''
	
	# try:
	# 	i = q.get(False) # passing False causes an exception to be thrown upon an empty queue
	# except Exception, e:
	# 	i = ""

	i = q.get()
	if i == "":
		mutex.acquire()
		print i
		mutex.release()

def connect(h=host, p=port):
    '''Connect to socket'''
    # for item in getAllIps():
        # try:    
        #     s.connect((item,p))
        #     return True # if you reach this line, the socket connected
        # except Exception, e:
        #     print("Unable to connect to socket at IP %s, port %d: %s" % (item, port, e))
        #     print("Please ensure socket is started.")
    try:
    	s.connect((h, p))
    	is_client = True
    	return True
    except Exception, e:
    	print("Unable to connect to socket at IP %s, port %d: error %s" % (h, port, e))
        print("Becoming server")
        s.close()

    # s.close()       
    return False # if you get here there was a failure to connect on any hosts

def startServer(h='', p=port, s=s):
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.bind((h, p))
		s.listen(1)
		is_server = True
		print "got here"
		return s
	except socket.error as msg:
		# print "got here"
		s.close()
		s = None
		return None

def serverLoop(s=s):
	''' endless loop to accept new clients
		upon accepting, inner loop to communicate '''
	while(True):
		conn, addr = s.accept()


def cleanup():
	signal.signal(signal.SIGINT, original_sigint) # restore the original handler just in case

	in_q.put("owaridayotto")
	s.close()
	p.join()
	sys.exit(1)

if __name__ == '__main__':
	newstdin = os.fdopen(os.dup(sys.stdin.fileno())) # copy the file handle for standard in
		
	if not connect(): # try to become client
		s = startServer() # try to become server
		# if s != None: 
		if not is_server:
			print("There was a problem becoming the server. Exiting")
			sys.exit(1)

	# if you get here, you can start to gather input
	p = Process(target=gatherInput, args=(newstdin,in_q)) # read input in new process with copy of standard in
	p.start()

	# be ready to close the socket and end the other process
	signal.signal(signal.SIGINT, cleanup)  

	# do sending and receiving 
	if is_client:
		# do client loop
		pass
	elif is_server:
		# do server loop
		pass


	cleanup()