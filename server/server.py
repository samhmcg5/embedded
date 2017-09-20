#!/usr/bin/env python3

"""
A simple echo server that handles some exceptions
"""

from pymongo import MongoClient
import socket

"""192.168.1.123"""
"""2000"""
mongo_client = MongoClient('localhost', 27017)
host = '192.168.159.100'
port = 50000
backlog = 5
size = 80000
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)

while 1:
    print ("Waiting for a connection: ")    
    client, address = s.accept()
    print ("Connected to: ")
    data = client.recv(size)
    if data:
        print ("Received:", str(data))
        db = mongo_client.test
        collect = db.testCollection
        post_data = {
            'title': 'Hello World'
        }
        recv = collect.find()
        for item in recv:
            print('Sending data:')
            print(item)
            s.send(item)

client.close()
