# Test 3
from server import Server
from pymongo import MongoClient
import bsonjs
import time
import json
import sys

def getargs(argv):
    args = []
    for arg in argv:
        args.append(arg)
    return args


def main():
    connection = MongoClient()
    db = connection.mag_database.mag_collection
    b = db.find_one()
    delivstate = b['state']
    delivbool = True
    pickupbool = True
    prevobjheld = 0
    ADDR = '192.168.1.123'
    PORT = 2001
    prevdelivstate = delivstate
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
        #print("-----------------------------------------------------------------------------")
        #print("Received Message: %s" % buf)
        #print()
        #msg_count += 1

        json_obj = json.loads(buf)
        objheld = json_obj['DELIV_SENSE']['OBJ']
        connection = MongoClient()   
        db = connection.mag_database.mag_collection
        b = db.find_one()
        delivstate = b['state']
        if(int(delivstate) is 0):
            if(int(objheld) is 1):
                if(pickupbool):
                    print("Object Successfully Picked Up")
                    pickupbool = False
                    prevobjheld = 1
            elif(int(objheld) is 0 and prevobjheld is 1):
                print("Object Lost")
        elif(int(delivstate) is 1):
            if(int(objheld) is 0):
                if(delivbool):
                    print("Object Successfully Delivered")
                    prevobjheld = 0
                    delivbool = False
        #print(delivstate)
        if(int(objheld) is 1 and int(delivstate) is 1):
            #print("turn off")
            delivbool = True
            prevobjheld = 1
            s.sendmsg('{"SEQ": 0, "ACTION": 0}!')

        elif(int(objheld) is 0 and int(delivstate) is 0):
            #print("turn on")
            pickupbool = True
            prevobjheld = 0
            s.sendmsg('{"SEQ": 0, "ACTION": 1}!')


    s.disconnect()

if __name__ == "__main__":
    main()
