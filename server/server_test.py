#!/usr/bin/env python3

"""
A simple echo server that handles some exceptions
"""

from pymongo import MongoClient
from pprint import pprint
import socket
import json


"""192.168.1.123"""
"""2000"""
    
dbcheck = None
while dbcheck is None:
    dbcheck = input("Is MongoDB Server on?")
mongo_client = MongoClient('localhost', 27017)
host = '192.168.1.123'
port = 2000
backlog = 5
length = None
size = 1
buffer = ""
data = ""
ack_msg = '{ "direction": 1 }'
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)

print ("Waiting for a connection: ")    
client, address = s.accept()
print ("Connected to WiFly: ")
while True:
    data = client.recv(size).decode()
    if not data:
        break
    buffer += data
    while True:
        if buffer == "*HELLO*":
            buffer = ""
            break
        if "!" not in buffer:
            break
        print("Received Message: ", buffer)
        buffer = buffer[:-1]
        json_obj = json.loads(buffer)
        pprint(json_obj)
        # store in db
        if 'store' in json_obj:
            db = mongo_client.test
            print("Storing Item: ")
            result = db.testCollection.insert_one(json_obj)
            if result.acknowledged:
                print("Sending acknowledgement: ")
                client.send(ack_msg.encode())
        # retrieve from db
        elif 'retrieve' in json_obj:
            print(json_obj['retrieve'])
            db = mongo_client.text
            json_str = '{ "store":' + str(json_obj['retrieve']) + ' }'
            json_obj_tmp = json.loads(json_str)
            pprint(json_obj_tmp)
            print("Retrieving Item: ")
            cursor = db.testCollection.find(json_obj_tmp)
            print("Retrieved Item")
            client.send(ack_msg.encode())
        buffer = ""
        break
client.close()
