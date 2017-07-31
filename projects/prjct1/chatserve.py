#/*********************************************************************
#** Author: Collin James
#** Date: 7/29/17
#** Description: CS 372 Project 1: Implements a chat server and client
#** in one file
#*********************************************************************/

## Uses ideas and minimal code from the following sources:
# https://stackoverflow.com/questions/8976962/is-there-any-way-to-pass-stdin-as-an-argument-to-another-process-in-python#8981813
# https://docs.python.org/2/library/socket.html
# https://stackoverflow.com/questions/18114560/python-catch-ctrl-c-command-prompt-really-want-to-quit-y-n-resume-executi#18115530
# https://docs.python.org/2/library/multiprocessing.html
# https://docs.python.org/2/library/queue.html#Queue.Empty
# https://docs.python.org/2/library/exceptions.html

from multiprocessing import Process, Queue, Lock
import Queue as DummyQueue
import time
import sys
from select import select
import os
import socket
import signal

# global variables
usage = "python chatserve.py [-help] [ip] [port]" # how to use
host = "127.0.0.1" # default IP to connect to 
port = 48834 # default port
out_q = Queue() # use the multiprocessing Queue (Fifo); for sending messages
wait_q = DummyQueue.LifoQueue() # needed for Lifo
in_q = Queue() # for closing gatherInput() process
mutex = Lock() # using to lock access to std_in, probably not necessary but being safe
TO = 0.5 # default timeout in seconds
original_sigint = signal.getsignal(signal.SIGINT) # save your original sigint handler # https://stackoverflow.com/questions/18114560/python-catch-ctrl-c-command-prompt-really-want-to-quit-y-n-resume-executi#18115530
PROC_EXIT = "owaridayotto" # special code to kill input process

class Chatter():
	'''Class to implement connecting to, sending to, receiving from server, and also
		performing the same functions as a server'''
	def __init__(self, port=port, handle="Anonymous"):
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.conn = None
		self.addr = None
		self.is_client = False
		self.is_server = False
		self.port = int(port)
		self.aiteiHandle = ""
		self.handle = handle
		self.gotAiteiHandle = False
		self.handleSent = False
		self.code = "namae:"

	### Client functions ###
	def connect(self, h=host):
	    '''Connect to socket as a client'''
	    try:
	    	self.s.settimeout(TO)
	    	self.s.connect((h, self.port))
	    	self.is_client = True
	    	return True
	    except Exception, e:
	    	print("...Unable to connect to socket at IP %s, port %d: error %s" % (h, self.port, e))
	        print("...Becoming server")
	        self.s.close()

	    return False # if you get here there was a failure to connect on any hosts

	def clientLoop(self):
		'''Loop until the user enters "\quit"; send and receive data '''
		self.s.settimeout(TO) # set socket to non-blocking with timeout
		self.sendHandle(self.s) # send your handle 
		# main loop. Try writing first; if queue is empty then read
		quit = False
		while not quit:
			try:
				# try sending first 
				if not wait_q.empty():
					msg = ""
					while not wait_q.empty():
						msg += wait_q.get()
					self.mysend(self.s, msg)
				if not out_q.empty():
					msg = ""
					while not out_q.empty():
						msg += out_q.get()
					self.mysend(self.s, msg)
				else:
					try:
						# then try receiving
						if self.checkAndReceive(self.s) == False:
							print "...Connection closed by server"
							quit = True
					except socket.timeout: # just try again
						pass
					except DummyQueue.Empty: # that's okay, try again
						pass
			except DummyQueue.Empty:
				time.sleep(TO)
			except socket.timeout, e:
				time.sleep(TO)
			except SystemExit, e: # user wants to quit, raise from mysend()
				quit = True
			except Exception, e:
				print e
		self._cleanup()

	### Server Functions ###
	def startServer(self, h=''):
		''' Try to become a server! '''
		try:
			self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.s.bind((h, self.port))
			self.s.listen(1)
			self.is_server = True
			print "...Chat server started"
			return True
		except socket.error as msg:
			self.s.close()
			self.s = None
			return False

	def serverLoop(self):
		''' endless loop to accept new clients
			upon accepting, inner loop to communicate '''
		# get new clients! this blocks
		self.acceptNewClients() # also sends handle
		self.conn.settimeout(TO) # make the socket non-blocking with timeout of TO
		print '...Connected with', self.addr
		while 1:
			try:
				if not wait_q.empty(): # try sending from wait_q first (messages that couldn't be sent earlier)
					msg = ""
					while not wait_q.empty(): # get all the waiting messages
						msg += wait_q.get()
					self.mysend(self.conn, msg)
				if not out_q.empty(): # now send "fresh" messages
					msg = ""
					while not out_q.empty():
						msg += out_q.get()
					self.mysend(self.conn, msg)
				else:
					try:
						if self.checkAndReceive(self.conn) == False: # if false, client has quit
							print "...client disconnected"
							self.acceptNewClients(True)
					except socket.timeout:
						pass
					except DummyQueue.Empty:
						pass
			except SystemExit, e:
				self.acceptNewClients(True) # True to close the client connection


	def acceptNewClients(self, close=False):
		''' listen for client connections and send handle '''
		# reset fresh application state
		self.gotAiteiHandle = False
		self.aiteiHandle = ""

		try:
			if close:
				self.conn.close()
			print "...accepting new clients"
			self.conn, self.addr = self.s.accept()
			# after connecting, send your handle
			self.sendHandle(self.conn)
		except socket.error, e:
			print "...socket closed, exiting"
			sys.exit(1)

	### Common Functions ###
	def sendHandle(self, s):
		'''special send for handles; "namae:_____" (namae is Japanese for name) '''
		if not self.handleSent:
			msg = self.code+self.handle
			s.send(msg)

	def mysend(self, s, msg):
		'''send all of the data, parsing for \quit command first '''
		totalsent = 0
		msglen = len(msg)
		if '\\quit\n' in msg: # does the user want to quit the chat?
			raise SystemExit
		# try to send until all data is sent
		while totalsent < msglen:
			try:
			    sent = s.send(msg[totalsent:])
			    if sent == 0:
			    	return 0
			except socket.timeout, e: # there was a timeout, so put the data back in a queue (FIFO)
				wait_q.put(msg)
				raise e
			except Exception, e: # some sore of agriegious error
				print e
				print "...connection broken"
				raise RuntimeError("socket connection broken")
			totalsent = totalsent + sent # set new total
		return totalsent

	def checkAndReceive(self, s):
		'''Try to read from the socket and print out messages from conversant'''
		ready_to_read, _, in_error = select([s], [], [s], TO)
		if ready_to_read: # we can try to read from the socket
			try:
				data = s.recv(1024)	
				if data[:-1] == "":
					return False
				# don't strip the trailing "\n" here because it was gathered with raw_input,
				# which doesn't add it like sys.stdin.readline()
				## determine who you're speaking to
				if not self.gotAiteiHandle and self.aiteiHandle == "":
					if self.code in data:
						# print data
						code, aitei = data.split(":")
					self.aiteiHandle = aitei
					self.gotAiteiHandle = True
					# let them know who you're speaking with!
					print "...Speaking with " + self.aiteiHandle
				else:
					# just in case messages are coming in really fast, make sure they don't
					# have any \n mixed in
					splitdata = data[:-1].split('\n') # [:-1] means strip trailing \n
					for part in splitdata: # print the 'lines'
						print self.aiteiHandle + "> " + part  			
			except socket.error, e: # socket is closed
				return False
			except socket.timeout, e: # socket timed out
				raise e
			return True	# should be good!

		elif in_error: # honestly not sure if I can get here
			print "in_error"

		return True

	def _cleanup(self):
		'''close down the input thread and the socket'''
		print "...quitting"
		in_q.put(PROC_EXIT) # exit gatherInput process
		self.s.close()
		mainCleanup()

	def cleanup(self, signum, frame):
		'''used as a sigint handler'''
		signal.signal(signal.SIGINT, original_sigint) # restore the original handler just in case
		self._cleanup()


def gatherInput(std_in, q):
	''' this will read the input for both client and server, put any received input in 
		a "buffer (out_q). It will only close when it receives the password'''
	# check for close command (no do while in Python)
	try:
		i = q.get(False) # passing False causes an exception to be thrown upon an empty queue
	except Exception, e:
		i = ""

	while i != PROC_EXIT:
		mutex.acquire() # not sure if I need to use mutex here for stdin, but I'm going to to be safe
		# select on std_in with timeout of TO seconds
		rlist, _, _ = select([std_in], [], [], TO)
		if rlist: # ready to read
			i = std_in.readline()
			if i != "":
				out_q.put(i)
		mutex.release()
		# check for close command again
		try:
			i = q.get(False)
		except Exception, e:
			i = ""

def getUserName():
	return raw_input("Please enter your chat handle: ")

def mainCleanup():
	''' called from the Chatter cleanup() command; shuts down input queue'''
	in_q.put(PROC_EXIT) # close down gatherInput Process
	p.join() # join the gatherInput thread
	sys.exit(1)


########### Main logic ############

if __name__ == '__main__':
	# copy the file handle for standard in; used with gatherInput() - # https://stackoverflow.com/questions/8976962/is-there-any-way-to-pass-stdin-as-an-argument-to-another-process-in-python#8981813
	newstdin = os.fdopen(os.dup(sys.stdin.fileno())) 

	# set default host and port
	h = host
	p = port
	# get user-defined host and port
	if len(sys.argv) >= 2:
		if "-h" in sys.argv[1]: # print usage info
			print usage
			sys.exit(0)
		else:
			h = sys.argv[1] # get host
		if len(sys.argv) >= 3:
			p = sys.argv[2] # get port
	# start the Chatter object
	ch = Chatter(port=p, handle=getUserName())
	if not ch.connect(h): # try to become client
		if not ch.startServer(): # if that fails, try to become server
			print("...There was a problem becoming the server. Exiting")
			sys.exit(1)

	# if you get here, you can start to gather input in a new process
	p = Process(target=gatherInput, args=(newstdin,in_q)) # read input in new process with copy of standard in
	p.start()

	# be ready to close the socket and end the other process
	signal.signal(signal.SIGINT, ch.cleanup)  

	# do sending and receiving 
	if ch.is_client:
		ch.clientLoop()
	elif ch.is_server:
		ch.serverLoop()

	# just in case, clean up if you get here
	mainCleanup()