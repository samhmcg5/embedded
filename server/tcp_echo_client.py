#!/usr/bin/env python3

"""
A simple echo client
"""

import socket

host = '192.168.159.100'
port = 50000
size = 1024
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))
print("Connected to server: ")
s.send(b"DB")
data = s.recv(size)
s.close()
print ('Received:', data)