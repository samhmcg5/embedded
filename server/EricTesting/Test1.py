# Test 1
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
	runtime = float(args[1]) + start
	expected_rate = float(args[2])
	tol = float(args[3])	

	curr_msg_time = 0.0
	prev_msg_time = time.time()

	msg_count = 0
	format_err_count = 0
	rate_err_count = 0

	total_rates = 0.0

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

	# Run for 60 seconds
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

		try:
			# Determine message rate and error
			curr_msg_time = time.time()
			diff = curr_msg_time - prev_msg_time
			prev_msg_time = curr_msg_time
			total_rates += diff
			error = abs(diff - expected_rate)
			check = True if error <= tol else False

			if not check:
				rate_err_count += 1

			print("Expected Message Rate: %fs | Actual Message Rate: %fs | Error: %fs | Error < Tolerance: %s" \
			 		% (expected_rate, diff, error, check))
			print()

			# Test if JSON format
			json_obj = json.loads(buf)

			# Test if correct fields are in message
			if "SEQ" not in json_obj:
				raise RuntimeError("Missing SEQ field!")

			if "SCAN_SENSE" not in json_obj:
				raise RuntimeError("Missing SCAN_SENSE field!")

			if "ZONE" not in json_obj['SCAN_SENSE']:
				raise RuntimeError("Missing ZONE field!")

			if "RED" not in json_obj['SCAN_SENSE']:
				raise RuntimeError("Missing RED field!")

			if "GREEN" not in json_obj['SCAN_SENSE']:
				raise RuntimeError("Missing GREEN field!")

			if "BLUE" not in json_obj['SCAN_SENSE']:
				raise RuntimeError("Missing BLUE field!")

		except ValueError:
			print("Message is not JSON format!")
			print("-----------------------------------------------------------------------------")
			print()
			format_err_count += 1

		except RuntimeError as err:
			print(err)
			print("-----------------------------------------------------------------------------")
			print()
			format_err_count += 1
		print("-----------------------------------------------------------------------------")
		print()

	print()
	print("---------------------")
	print("STATISTICS FOR TEST 1")
	print("---------------------")
	print()
	print("Ratio of formatting errors to total messages: %d/%d" % (format_err_count, msg_count))
	print()
	print("Formatting success rate: %f %%" % (float(1 - float(format_err_count/msg_count))*100))
	print()
	print("Ratio of message rate errors to total messages: %d/%d" % (rate_err_count, msg_count))
	print()
	print("Average Message Rate: %fs" % (float(total_rates/msg_count)))
	print()
	print("Message rate success rate: %f %%" % (float(1 - float(rate_err_count/msg_count))*100))
	print()

	s.disconnect()

if __name__ == "__main__":
	main()