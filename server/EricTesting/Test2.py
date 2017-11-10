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
			buf += s.recvmsg()
			if "*HELLO*" in buf:
				buf = ""

		buf = buf[:-1]
		print("-----------------------------------------------------------------------------")
		print("Received Message: %s" % buf)
		print()
		msg_count += 1

		# Determine current zone
		json_obj = json.loads(buf)
		seq = json_obj['SEQ']
		update_zone = 0
		# Check for expected zone
		if seq == zone1:
			expected_zone = 0
			update_zone = 1
		elif seq <= zone2 and seq > zone1:
			expected_zone = 1
			update_zone = 1
			if seq == zone2:
				update_zone = 2
		elif seq <= zone3 and seq > zone2:
			expected_zone = 2
			update_zone = 2
			if seq == zone3:
				update_zone = 3
		elif seq > zone3:
			expected_zone = 3
			update_zone = 3 

		zone = json_obj['SCAN_SENSE']['ZONE']
		check = True if zone == expected_zone else False

		if not check:
			zone_err_count += 1
		
		print("Expected Zone: %s | Actual Zone: %s | Success: %s" % (expected_zone, zone, check))	
		print()
		msg = '{ "SEQ": ' + str(seq+1) +  ', "ZONE": ' + str(update_zone) + ', "ACTION": 2 }!'
		print("Sending Message: %s" % msg)
		s.sendmsg(msg)

		print("-----------------------------------------------------------------------------")
		print()

	print()
	print("---------------------")
	print("STATISTICS FOR TEST 2")
	print("---------------------")
	print()
	print("Ratio of zone errors to total messages: %d/%d" % (zone_err_count, msg_count))
	print()
	print("Zone success rate: %f %%" % (float(1 - float(zone_err_count/msg_count))*100))
	print()

	s.disconnect()

if __name__ == "__main__":
	main()
