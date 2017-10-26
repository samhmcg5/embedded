# Test 3
from server import Server
import time
import json
import sys

def main():
	ADDR = '192.168.1.123'
	PORT = 2001

	# Instantiate server
	s = Server(ADDR, PORT)
	s.socket_init()
	s.start()

	# Connect to client
	print("Waiting to connect ...")
	s.connect()
	print("Connected to wifly at %s:%s" % (s.client_addr[0], s.client_addr[1]))
	print()

	while True:

		# Receive messages from client
		buf = ""
		while '!' not in buf:
			buf += s.recvmsg()
			if "*HELLO*" in buf:
				buf = ""

		buf = buf[:-1]
		print("-----------------------------------------------------------------------------")
		print("Received Message: %s" % buf)
		print()
		#msg_count += 1

		json_obj = json.loads(buf)
		objheld = json_obj['DELIV_SENSE']['OBJ']
		if(objheld):
			print("Object Held")
		else:
			print("Object Not Held")


	s.disconnect()

if __name__ == "__main__":
	main()
