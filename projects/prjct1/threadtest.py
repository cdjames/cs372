# from multiprocessing import Process, Queue
from threading import Thread
from Queue import Queue
import time
from sys import stdin
from select import select

class InputGetter(Thread):
	"""docstring for InputGetter"""
	
	def __init__(self, q, std_in):
		super(InputGetter, self).__init__()
		self.q = q
		self.done = False
		self.std_in = std_in
		self.timeout = 0.5
		
	def run(self):
		try:
			i = self.q.get(False)
		except Exception, e:
			i = ""
		while i != "owaridayotto":
			if i != "":
				rlist, _, _ = select([self.std_in], [], [], self.timeout)
				if rlist:
					i = self.std_in.readline()
					print i
				# raw_input("hi")
			else:
				print "waiting"
			try:
				i = self.q.get(False)
			except Exception, e:
				i = ""



if __name__ == '__main__':
	q = Queue()
	q.put("hello")
	ig = InputGetter(q, stdin)
	ig.start()
	# time.sleep(0.5)
	q.put("yikes")
	q.put("owaridayotto")
	# ig.terminate()
	ig.join()