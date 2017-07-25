from multiprocessing import Process, Queue
import time
# from sys import stdin

class InputGetter(Process):
	"""docstring for InputGetter"""
	
	def __init__(self, q):
		super(InputGetter, self).__init__()
		self.q = q
		self.done = False
		
	def run(self):
		try:
			i = self.q.get(False)
		except Exception, e:
			i = ""
		while i != "owaridayotto":
			if i != "":
				# i = std_in.readline()
				print i
				# raw_input("hi")
			else:
				print "waiting"
			try:
				i = self.q.get(False)
			except Exception, e:
				i = ""

	def terminate(self):
		self.done = True


if __name__ == '__main__':
	q = Queue()
	q.put("hello")
	ig = InputGetter(q)
	ig.start()
	time.sleep(0.5)
	q.put("yikes")
	q.put("owaridayotto")
	# ig.terminate()
	# ig.join()