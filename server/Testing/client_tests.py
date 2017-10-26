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
		print("Test connect ...")
		test_connect()
	elif "-s" in args:
		print("Test send recv msg ...")
		test_send_recv_msg()

def test_connect():
	cli = Client(srvf.IP_ADDR, srvf.scan_sense_port)
	cli.socket_init()
	cli.connect()

def test_send_recv_msg():
	cli = Client(srvf.IP_ADDR, srvf.scan_sense_port)
	cli.socket_init()
	cli.connect()
	buf = ""
	while '!' not in buf:
		buf += cli.recvmsg()
	print(buf)
	cli.sendmsg(buf)

if __name__ == '__main__':
	main()