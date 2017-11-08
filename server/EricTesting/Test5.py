# Test 5
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

	# Get args
	args = getargs(sys.argv)
	runtime = float(args[1])
	zone1_quota = [float(args[2]), float(args[3]), float(args[4])] 
	zone2_quota = [float(args[5]), float(args[6]), float(args[7])]
	zone3_quota = [float(args[8]), float(args[9]), float(args[10])]

	zone1_actual = [0, 0, 0]
	zone2_actual = [0, 0, 0]
	zone3_actual = [0, 0, 0]
	zones_actual = [zone1_actual, zone2_actual, zone3_actual]

	zone1_err_count = [0, 0, 0]
	zone2_err_count = [0, 0, 0]
	zone3_err_count = [0, 0, 0]  
	
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
		
		# Determine current color
		json_obj = json.loads(buf)
		seq = json_obj['SEQ']
		red = json_obj['SCAN_SENSE']['RED']
		green = json_obj['SCAN_SENSE']['GREEN']
		blue = json_obj['SCAN_SENSE']['BLUE']
		zone = json_obj['SCAN_SENSE']['ZONE']
		
		if red == 1 and green == 0 and blue == 0:
			zones_actual[zone - 1][0] += 1
		elif red == 0 and green == 1 and blue == 0:
			zones_actual[zone - 1][1] += 1
		elif red == 0 and green == 0 and blue == 1:
			zones_actual[zone - 1][2] += 1
	
		if seq < 10:
			zone = 1
			msg = '{ "SEQ": ' + str(seq+1) +  ', "ZONE": ' + str(zone) +  ', "ACTION": 2 }!'
		elif seq < 20 and seq >= 10:
			zone = 2
			msg = '{ "SEQ": ' + str(seq+1) +  ', "ZONE": ' + str(zone) +  ', "ACTION": 2 }!'
		elif seq >= 20 and seq < 30:
			zone = 3
			msg = '{ "SEQ": ' + str(seq+1) +  ', "ZONE": ' + str(zone) +  ', "ACTION": 2 }!'
		elif seq >= 30:
			zone = 3
			msg = '{ "SEQ": ' + str(seq+1) +  ', "ZONE": ' + str(zone) +  ', "ACTION": 0 }!'

		print("Sending Message: %s" % msg)
		s.sendmsg(msg)

		print("-----------------------------------------------------------------------------")
		print()

	zone1_err_count = [zone1_quota[0] - zone1_actual[0], zone1_quota[1] - zone1_actual[1], zone1_quota[2] - zone1_actual[2]] 
	zone2_err_count = [zone2_quota[0] - zone2_actual[0], zone2_quota[1] - zone2_actual[1], zone2_quota[2] - zone2_actual[2]]
	zone3_err_count = [zone3_quota[0] - zone3_actual[0], zone3_quota[1] - zone3_actual[1], zone3_quota[2] - zone3_actual[2]]

	print()
	print("---------------------")
	print("STATISTICS FOR TEST 5")
	print("---------------------")
	print()
	print("ZONE 1 RESULTS")
	print("--------------")
	print("Quota:")
	print(zone1_quota)
	print("Actual:")
	print(zone1_actual)
	print("Error:")
	print(zone1_err_count)
	print()
	print("ZONE 2 RESULTS:")
	print("--------------")
	print("Quota:")
	print(zone2_quota)
	print("Actual:")
	print(zone2_actual)
	print("Error:")
	print(zone2_err_count)
	print()
	print("ZONE 3 RESULTS:")
	print("--------------")
	print("Quota:")
	print(zone3_quota)
	print("Actual:")
	print(zone3_actual)
	print("Error:")
	print(zone3_err_count)
	print()

	s.disconnect()

if __name__ == "__main__":
	main()