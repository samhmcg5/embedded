# Test 3
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
	aggregate_runtime = runtime
	tol = float(args[2])
	
	msg_count = 0
	dist_err_count = 0 
	total_err = 0.0
	total_dist = 0.0

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

	for dist in args[3:len(args)]: 
		# Run for x seconds
		while time.clock() <= aggregate_runtime:

			# Test for specific distance
			expected_distance = float(dist)

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

			json_obj = json.loads(buf)
			actual_distance = json_obj['DELIV_SENSE']['IRDIST']
			total_dist += actual_distance
			error = abs(actual_distance - expected_distance) 
			total_err += error
			check = True if error <= tol else False

			if not check:
				dist_err_count += 1

			print("Expected Distance: %f cm | Actual Distance: %f cm | Error: %f cm | Error < Tolerance: %s" \
			  		% (expected_distance, actual_distance, error, check))	
			print()

			print("-----------------------------------------------------------------------------")
			print()

		print()
		print("---------------------")
		print("STATISTICS FOR TEST 3")
		print("---------------------")
		print()
		print("Ratio of distance errors to total messages: %d/%d" % (dist_err_count, msg_count))
		print()
		print("Average detected distance: %f cm" % (float(total_dist/msg_count)))
		print()
		print("Average distance error: %f cm" % (float(total_err/msg_count)))
		print()
		print("Expected distance success rate: %f %%" % (float(1 - float(dist_err_count/msg_count))*100))
		print()

		dist_err_count = 0
		msg_count = 0
		total_dist = 0.0
		total_err = 0.0
		aggregate_runtime += runtime

	s.disconnect()

if __name__ == "__main__":
	main()
