# Test 2
from server import Server
import time
import json
import sys

def getargs(argv):
	args = []
	for arg in argv:
		args.append(arg)
	return args 

def main():

	# Start stopwatch
	start = time.clock()
        
	args = getargs(sys.argv)
	runtime = float(args[1]) + start

	msg_count = 0
	expected_zone = 0
	zone1 = 0
	zone2 = 10
	zone3 = 20
	zone_err_count = 0 

	ADDR = '192.168.1.123'
	PORT = 2003

	# Instantiate server
	s = Server(ADDR, PORT)
	s.socket_init()
	s.start()
	
	# Connect to client
	print("Waiting to connect ...")
	s.connect()
	print("Connected to wifly at %s:%s" % (s.client_addr[0], s.client_addr[1]))
	print()

	# Run for x seconds
	while time.clock() <= runtime:

		# Receive messages from client
		buf = ""
		while '!' not in buf:
			#print(buf)
			buf += s.recvmsg()
			if "*HELLO*" in buf:
				buf = ""

		buf = buf[:-1]
		print("-----------------------------------------------------------------------------")
		print("Received Message: %s" % buf)
		print()

if __name__ == '__main__':
	main()