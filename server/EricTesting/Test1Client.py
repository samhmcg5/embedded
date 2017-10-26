# Mock Client Test 1
from client import Client
from time import sleep

def main():
	ADDR = '192.168.1.123'
	PORT = 2003

	bad_msg1 = "Not JSON!"
	bad_msg2 = '{ SCAN_SENSE": { "ZONE": 0, "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
	bad_msg3 = '{ "SEQ": 0, ZONE": 0, "RED": 0, "GREEN": 0, "BLUE": 0 }!'
	bad_msg4 = '{ "SEQ": 0, "SCAN_SENSE": { "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
	bad_msg5 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "GREEN": 0, "BLUE": 0 } }!'
	bad_msg6 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "RED": 0, "BLUE": 0 } }!'
	bad_msg7 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "RED": 0, "GREEN": 0 } }!'
					
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
	print("Sending Message: %s" % bad_msg1)
	c.sendmsg(bad_msg1)
	sleep(1)
	print("Sending Message: %s" % bad_msg2)
	c.sendmsg(bad_msg2)
	sleep(1)
	print("Sending Message: %s" % bad_msg3)
	c.sendmsg(bad_msg3)
	sleep(1)
	print("Sending Message: %s" % bad_msg4)
	c.sendmsg(bad_msg4)
	sleep(1)
	print("Sending Message: %s" % bad_msg5)
	c.sendmsg(bad_msg5)
	sleep(1)
	print("Sending Message: %s" % bad_msg6)
	c.sendmsg(bad_msg6)
	sleep(1)
	print("Sending Message: %s" % bad_msg7)
	c.sendmsg(bad_msg7)
	sleep(1)

	# Send messages to server
	while(True):
		msg = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 0, "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
		print("Sending Message: %s" % msg)
		c.sendmsg(msg)
		sleep(2)

if __name__ == "__main__":
	main()