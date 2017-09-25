#!/usr/bin/env python3

"""
A simple echo client
"""

import socket

host = '192.168.1.123'
port = 2000
size = 1024
filename = "client_log.txt"
file = open(filename, 'w')
count = 0;

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))

file.write("Connected to server: \n")
while True:
	json_str = input("Message to send to server: ") 
	file.write("Sending to server: \n")
	s.send(json_str.encode())
file.close()