# Client testing
from client import *
import sys
import server_fields as srvf

def getargs(argv):
    args = {}
    for arg in argv:
        if arg[0] in '-':
        	args[arg] = arg
    return args

def main():
	args = getargs(sys.argv)
	if "-c" in args:
		print("Here")
		test_connect()
	#elif "-s" in args:
		#test_sendmsg()

def test_connect():
	cli = Client(srvf.IP_ADDR, srvf.scan_sense_port)
	cli.socket_init()
	cli.connect()

if __name__ == '__main__':
	main()