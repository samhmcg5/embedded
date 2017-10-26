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

    args = getargs(sys.argv)
    delivstate = args[1]
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

    while True:

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
        #msg_count += 1

        json_obj = json.loads(buf)
        objheld = json_obj['DELIV_SENSE']['OBJ']
        if(int(objheld) is 1 and int(delivstate) is 1):
            print("turn off")
            s.sendmsg('{"SEQ": 0, "ACTION": 0}!')

        if(int(objheld) is 0 and int(delivstate) is 0):
            print("turn on")
            s.sendmsg('{"SEQ": 0, "ACTION": 1}!')


    s.disconnect()

if __name__ == "__main__":
    main()
