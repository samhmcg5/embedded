#!/usr/bin/env python3

"""
A simple echo client
"""

import socket

host = '192.168.1.123'
port = 2000
size = 1024
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))
print("Connected to server: ")
while True:
	json_str = input("Send a json message to server: ")
	s.send(json_str.encode())
	data = s.recv(size)
	print ('Received:', data.decode())