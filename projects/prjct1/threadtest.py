# from multiprocessing import Process, Queue
from threading import Thread, Lock
from Queue import Queue
import time
from sys import stdin
from select import select

out_q = Queue()
mutex = Lock()

class InputGetter(Thread):
	"""docstring for InputGetter"""
	
	def __init__(self, in_q, std_in):
		super(InputGetter, self).__init__()
		self.in_q = in_q
		self.std_in = std_in
		self.timeout = 0.5
		
	def run(self):
		try:
			i = self.in_q.get(False)
		except Exception, e:
			i = ""
		while i != "owaridayotto":
			if i != "":
				mutex.acquire()
				rlist, _, _ = select([self.std_in], [], [], self.timeout)
				if rlist:
					i = self.std_in.readline()
					# print i
					out_q.put(i)
				mutex.release()
				# raw_input("hi")
			# else:
			# 	print "waiting"
			# mutex.acquire()
			# out_q.put("nothing")
			# mutex.release()
			try:
				i = self.in_q.get(False)
			except Exception, e:
				i = ""



if __name__ == '__main__':
	in_q = Queue()
	in_q.put("hello")
	ig = InputGetter(in_q, stdin)
	ig.start()
	done = False
	while not done:
		try:
			mutex.acquire()
			msg = out_q.get(False)
			mutex.release()
			print msg
			done = True
		except Exception, e:
			mutex.release()
		
	# time.sleep(0.5)
	in_q.put("yikes")
	in_q.put("owaridayotto")
	# ig.terminate()
	ig.join()