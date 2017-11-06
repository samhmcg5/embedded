# Mock Client Test 3
from client import Client
from time import sleep
import json

def main():
	ADDR = '192.168.1.123'
	PORT = 2003
				
	# Instantiate client
	c = Client(ADDR, PORT)
	c.socket_init()
	
	# Connect to Server
	print("Waiting to connect ...")
	c.connect()
	print("Connected to server at %s:%s" % (ADDR, PORT))

	msg = "*HELLO*"
	print("Sending Message: %s" % msg)
	c.sendmsg(msg)
	sleep(1)
	
	color = 0
	# Send messages to server
	while(True):
		if color == 0:
			msg = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "RED": 1, "GREEN": 0, "BLUE": 0 } }!'
		elif color == 1:
			msg = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "RED": 0, "GREEN": 1, "BLUE": 0 } }!'
		elif color == 2:
			msg = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "RED": 0, "GREEN": 0, "BLUE": 1 } }!'
		elif color == 3:
			msg = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
		
		print("Sending Message: %s" % msg)
		c.sendmsg(msg)
		recv = c.recvmsg()
		print("Received Message: %s" % recv)
		json_obj = json.loads(recv)
		seq = json_obj['SEQ']

		if seq >= 0 and seq < 10:
			color = 0
		if seq >= 10 and seq < 20:
			color = 1
		if seq >= 20 and seq < 30:
			color = 2
		if seq >= 30:
			color = 3

		sleep(1)

if __name__ == "__main__":
	main()