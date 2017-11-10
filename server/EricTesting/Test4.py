# Test 4
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

	msg_count = 0
	expected_colors = ['RED', 'GREEN', 'BLUE', 'NO COLOR']
	expected_color = 0 
	color_err_count = 0 

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

	first = True
	for expected_color in [0, 1, 2, 3]:
		# Run for x seconds
		while time.clock() <= aggregate_runtime:

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

			if first:
				msg = '{ "SEQ": 1, "ZONE": 1, "ACTION": 1, "XPOS": 0 }!' 
				print("Sending Message: %s" % msg)
				s.sendmsg(msg)
				first = False
			else:
				# Determine current color
				json_obj = json.loads(buf)
				seq = json_obj['SEQ']
				red = json_obj['SCAN_SENSE']['RED']
				green = json_obj['SCAN_SENSE']['GREEN']
				blue = json_obj['SCAN_SENSE']['BLUE']
				
				actual_color = None
				if red == 1 and green == 0 and blue == 0:
					actual_color = 'RED'
				elif red == 0 and green == 1 and blue == 0:
					actual_color = 'GREEN'
				elif red == 0 and green == 0 and blue == 1:
					actual_color = 'BLUE'
				else:
					actual_color = 'NO COLOR'
					
				check = None
				# Check for expected color
				if expected_color == 0:
					# RED
					check = True if red == 1 and green == 0 and blue == 0 else False
					if not check:
						color_err_count += 1
				elif expected_color == 1:
					# GREEN
					check = True if red == 0 and green == 1 and blue == 0 else False
					if not check:
						color_err_count += 1
				elif expected_color == 2:
					# BLUE
					check = True if red == 0 and green == 0 and blue == 1 else False
					if not check:
						color_err_count += 1
				elif expected_color == 3:
					# NO COLOR
					check = True if red == 0 and green == 0 and blue == 0 else False
					if not check:
						color_err_count += 1

				print("Expected Color: %s | Actual Color: %s | Success: %s" % (expected_colors[expected_color], actual_color, check))	
				print()
				msg = '{ "SEQ": ' + str(seq+1) +  ', "ZONE": 1, "ACTION": 2, "XPOS": ' + str(seq+1) + ' }!'
				print("Sending Message: %s" % msg)
				s.sendmsg(msg)

				print("-----------------------------------------------------------------------------")
				print()

		print()
		print("---------------------")
		print("STATISTICS FOR TEST 4")
		print("---------------------")
		print()
		print("COLOR: %s" % expected_colors[expected_color])
		print()
		print("Ratio of color errors to total messages: %d/%d" % (color_err_count, msg_count))
		print()
		print("Color success rate: %f %%" % (float(1 - float(color_err_count/msg_count))*100))
		print()

		msg_count = 0
		color_err_count = 0
		aggregate_runtime += runtime

	s.disconnect()

if __name__ == "__main__":
	main()