# Mock Client Test 3
from client import Client
from time import sleep

def main():
	ADDR = '192.168.1.126'
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
	
	# Send messages to server
	while(True):
		msg = '{ "SEQ": 0, "DELIV_SENSE": { "IRDIST": 15, "OBJ": 0} }!'
		print("Sending Message: %s" % msg)
		c.sendmsg(msg)
		sleep(2)
		msg = '{ "SEQ": 0, "DELIV_SENSE": { "IRDIST": 15, "OBJ": 1} }!'
		print("Sending Message: %s" % msg)
		c.sendmsg(msg)
		sleep(2)

if __name__ == "__main__":
	main()