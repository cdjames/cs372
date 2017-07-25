from multiprocessing import Process, Queue

class InputGetter(Process):
	"""docstring for InputGetter"""
	
	def __init__(self, q):
		super(InputGetter, self).__init__()
		self.q = q
		self.done = False
		
	def run(self):
		i = self.q.get() or ""
		while i != "owaridayotto":
			if i != "":
				print i
			i = self.q.get() or ""

	def terminate(self):
		self.done = True


if __name__ == '__main__':
	q = Queue()
	q.put("hello")
	q.put("yikes")
	ig = InputGetter(q)
	q.put("owaridayotto")
	ig.start()
	# ig.terminate()
	# ig.join()