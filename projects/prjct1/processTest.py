# https://stackoverflow.com/questions/8976962/is-there-any-way-to-pass-stdin-as-an-argument-to-another-process-in-python#8981813
# https://docs.python.org/2/library/socket.html
# https://stackoverflow.com/questions/18114560/python-catch-ctrl-c-command-prompt-really-want-to-quit-y-n-resume-executi#18115530

from multiprocessing import Process, Queue, Lock
import Queue as DummyQueue
import time
import sys
from select import select
import os
import socket
import signal

host = "127.0.0.1"
port = 48834
out_q = Queue()
in_q = Queue()
mutex = Lock()
timeout = 1
original_sigint = signal.getsignal(signal.SIGINT)

class Chatter():
	"""docstring for Chatter"""
	def __init__(self, port=port):
		# super(Chatter, self).__init__()	
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.conn = None
		self.addr = None
		self.is_client = False
		self.is_server = False
		self.port = int(port)

	def clientLoop(self, q):
		''' this will look for something in the out_q and send to a conversant '''
		
		# try:
		# 	i = q.get(False) # passing False causes an exception to be thrown upon an empty queue
		# except Exception, e:
		# 	i = ""

		i = q.get()
		if i == "":
			self.mutex.acquire()
			print i
			self.mutex.release()

	def connect(self, h=host):
	    '''Connect to socket'''
	    # for item in getAllIps():
	        # try:    
	        #     s.connect((item,p))
	        #     return True # if you reach this line, the socket connected
	        # except Exception, e:
	        #     print("Unable to connect to socket at IP %s, port %d: %s" % (item, port, e))
	        #     print("Please ensure socket is started.")
	    try:
	    	self.s.connect((h, self.port))
	    	self.is_client = True
	    	self.s.settimeout(1)
	    	return True
	    except Exception, e:
	    	print("Unable to connect to socket at IP %s, port %d: error %s" % (h, port, e))
	        print("Becoming server")
	        self.s.close()

	    # s.close()       
	    return False # if you get here there was a failure to connect on any hosts

	def startServer(self, h=''):
		try:
			self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.s.bind((h, self.port))
			self.s.listen(1)
			self.is_server = True
			print "server started"
			return True
		except socket.error as msg:
			# print "got here"
			self.s.close()
			self.s = None
			return False

	def serverLoop(self):
		''' endless loop to accept new clients
			upon accepting, inner loop to communicate '''
		self.conn, self.addr = self.s.accept()
		self.conn.settimeout(1)
		print 'Connected by', self.addr
		while 1:
			try:
				if self.mySendReceive(self.conn) == False:
					print "accepting new clients"
					self.conn, self.addr = self.s.accept()
			except socket.timeout:
				pass
			except DummyQueue.Empty:
				pass
			except SystemExit, e:
				# mysend(self.conn)
				self.conn.close()
				print "accepting new clients"
				self.conn, self.addr = self.s.accept()
				# self.s.shutdown(0)
				# self.s = None
				# if self.startServer():
				# 	self.conn, self.addr = self.s.accept()
				# else:
				# 	"could not become server"
				# 	self._cleanup()

			# data = self.conn.recv(1024)
			# if not data: break
			# self.conn.sendall(data)
		# self.conn.close()

	def clientLoop(self):
		# print self.mySendReceive(self.s)
		self.s.settimeout(1)
		while 1:
			try:
				if self.mySendReceive(self.s) == False:
					break

			except DummyQueue.Empty:
				print "queue empty"
				time.sleep(1)
				pass
			except socket.timeout, e:
				print "socket timeout"
				time.sleep(1)
				pass
			except SystemExit, e:
				self._cleanup()
			except Exception, e:
				print e
			# pass
		# while self.mySendReceive(self.s) != False:
		# 	pass

	def mySendReceive(self, s):
		# print "mySendReceive"
		ready_to_read, ready_to_write, in_error = \
		           select(
		              [s],
		              [s],
		              [s],
		              timeout)
		if ready_to_read:
			# if self.is_server:
			# 	print "in ready_to_read"
			try:
				data = s.recv(1024)
				print data + " 103"
				if data == "":
					return False
			except Exception, e:
				return False
			except socket.timeout, e:
				raise e
			return True	
		elif ready_to_write:	
			# if self.is_server:
			# 	print "in ready_to_write"
			try:
				s_data = out_q.get(False)
			except Exception, e:
				raise e
			except socket.timeout, e:
				raise e
				# print "returning, nothing in out queue"
				# return False
			# print s_data + "retrieved from queue"
			if s_data == '\\quit\n':
				print "exiting chat"
				raise SystemExit
			try:
				if self.mysend(s, s_data) == 0:
					return False
			except Exception, e:
				return False
			except socket.timeout, e:
				raise e
			try:
				data = s.recv(1024) # make sure socket is still open
				print data + " 110"
				if data == "":
					return False
			except socket.timeout, e:
				raise e
			except Exception, e:
				raise e

			return True
		elif in_error:
			print "in_error"
	        # chunks = []
	        # bytes_recd = 0
	        # while bytes_recd < MSGLEN:
	        #     print "bytes recd=%d" % (bytes_recd) 
	        #     chunk = s.recv(min(MSGLEN - bytes_recd, 2048))
	        #     if chunk == '':
	        #         raise RuntimeError("socket connection broken")
	        #     chunks.append(chunk)
	        #     bytes_recd = bytes_recd + len(chunk)
	        #     return ''.join(chunks)

	def mysend(self, s, msg):
		totalsent = 0
		msglen = len(msg)
		# print "msglen=%d" % (msglen)
		while totalsent < msglen:
			try:
			    sent = s.send(msg[totalsent:])
			    # print "sent=%d" % (sent)
			    if sent == 0:
			    	return 0
			except socket.timeout, e:
				raise e
			except Exception, e:
			    # if sent == 0:
				print "connection broken"
				raise RuntimeError("socket connection broken")
				# return 0
			# try:
			# 	data = s.recv(32)
			# 	print "client returned %s" % data
			# except Exception, e:
			# 	print "client disconnected"
			# 	return False
			totalsent = totalsent + sent
			# print "total sent=%d" % (totalsent)
		return totalsent

	def _cleanup(self):
		in_q.put("owaridayotto")
		self.s.close()
		mainCleanup()

	def cleanup(self, signum, frame):
		signal.signal(signal.SIGINT, original_sigint) # restore the original handler just in case

		# in_q.put("owaridayotto")
		# self.s.close()
		# mainCleanup()
		self._cleanup()


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

def mainCleanup():
	in_q.put("owaridayotto")
	p.join()
	sys.exit(1)

if __name__ == '__main__':
	newstdin = os.fdopen(os.dup(sys.stdin.fileno())) # copy the file handle for standard in
	p = port
	if len(sys.argv) >= 2:
		p = sys.argv[1]

	ch = Chatter(port=p)
	if not ch.connect(): # try to become client
		if not ch.startServer(): # try to become server
			print("There was a problem becoming the server. Exiting")
			sys.exit(1)

	# if you get here, you can start to gather input
	p = Process(target=gatherInput, args=(newstdin,in_q)) # read input in new process with copy of standard in
	p.start()

	# be ready to close the socket and end the other process
	signal.signal(signal.SIGINT, ch.cleanup)  

	# do sending and receiving 
	if ch.is_client:
		# do client loop
		ch.clientLoop()
	elif ch.is_server:
		ch.serverLoop()


	mainCleanup()