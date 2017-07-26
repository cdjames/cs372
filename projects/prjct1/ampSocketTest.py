import socket
import select
import sys

# automatically find available IPs to connect to a local socket
def getAllIps():
    # from https://stackoverflow.com/questions/270745/how-do-i-determine-all-of-my-ip-addresses-when-i-have-multiple-nics
    # print [i[4][0] for i in socket.getaddrinfo(socket.gethostname(), None)]
    # ips = ['123.45.56.67'] # testing bad IP
    ips = []
    for i in socket.getaddrinfo(socket.gethostname(), None):
       if ":" not in i[4][0]:
        ips.append(i[4][0])
    ips.append('127.0.0.1')
    return ips 

# global variables
# socket.setdefaulttimeout(0.1) # set timeout on socket connection to 1/10 of a second
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
# host = '127.0.0.1'
# host = '10.34.150.19'
# host = '192.168.1.11'
host=getAllIps()[0]
port = 48834
MSGLEN = 512


# automatically try all interfaces to the local socket
def connect(h=host, p=port):
    '''Connect to socket'''
    for item in getAllIps():
        try:    
            s.connect((item,p))
            return True # if you reach this line, the socket connected
        except Exception, e:
            print("Unable to connect to socket at IP %s, port %d: %s" % (item, port, e))
            print("Please ensure socket is started.")
    s.close()       
    return False # if you get here there was a failure to connect on any hosts

def send_rcv_CAN_cmd(cmd):
        '''Send a message and print the return message'''
        s.send(cmd + '\n')
        data = s.recv(32)
        # check for connection errors and save to self.connected
        if '-001 error' in data:
            print "error"
            connected = False
        print "cmd %s: %s" %(cmd,data)

def mysend(msg):
        totalsent = 0
        msglen = len(msg)
        print "msglen=%d" % (msglen)
        while totalsent < msglen:
            sent = s.send(msg[totalsent:])
            print "sent=%d" % (sent)
            if sent == 0:
                print "connection broken"
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent
            print "total sent=%d" % (totalsent)
        return totalsent

def myreceive():
    ready_to_read, ready_to_write, in_error = \
               select.select(
                  [s],
                  [],
                  [],
                  1)
    if (ready_to_read):
        data = s.recv(32)
        return data
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

### Run tests (manual style)
if __name__ == '__main__':
    print "connecting"
    connected = connect()
    # if connected:
    #     print "sending"
    #     mysend("hello")
    #     print "received %s" % (myreceive())
    #     s.shutdown(0)
    #     s.close()
