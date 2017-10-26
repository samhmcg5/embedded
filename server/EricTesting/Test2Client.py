# Mock Client Test 2
from client import Client
from time import sleep
import json

def main():
	ADDR = '192.168.1.124'
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

	zone = 0
	seq = 0

	# Send messages to server
	while(True):
		msg = '{ "SEQ": ' + str(seq) + ', "SCAN_SENSE": { "ZONE": ' + str(zone) + ', "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
		print("Sending Message: %s" % msg)
		c.sendmsg(msg)
		
		buf = ""
		while '!' not in buf:
			buf += c.recvmsg()
		buf = buf[:-1]
		print("Received Message %s" % buf)
		
		json_obj = json.loads(buf)
		seq = json_obj['SEQ']
		zone = json_obj['ZONE']
		
		sleep(1)

if __name__ == "__main__":
	main()