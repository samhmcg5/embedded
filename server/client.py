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

ZONE1_DEFAULT_1 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 1, "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
ZONE1_DEFAULT_2 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 1, "RED": 1, "GREEN": 1, "BLUE": 1 } }!'

ZONE2_DEFAULT_1 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 2, "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
ZONE2_DEFAULT_2 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 2, "RED": 1, "GREEN": 1, "BLUE": 1 } }!'

ZONE3_DEFAULT_1 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 3, "RED": 0, "GREEN": 0, "BLUE": 0 } }!'
ZONE3_DEFAULT_2 = '{ "SEQ": 0, "SCAN_SENSE": { "ZONE": 3, "RED": 1, "GREEN": 1, "BLUE": 1 } }!'

DELIV_NAV_STATUS_1 = '{ "SEQ": 0, "DELIV_NAV": { "STATUS": 0 } }!'
DELIV_NAV_STATUS_2 = '{ "SEQ": 0, "DELIV_NAV": { "STATUS": 1 } }!'

DELIV_NAV_POS_1 = '{ "SEQ": 0, "DELIV_NAV": { "X": 0, "Y": 0 } }!'
DELIV_NAV_POS_2 = '{ "SEQ": 0, "DELIV_NAV": { "X": 1, "Y": 1 } }!'

DELIV_NAV_SPD_1 = '{ "SEQ": 0, "DELIV_NAV": { "RIGHT_DIR": 0, "LEFT_DIR": 0, "RIGHT_SPEED": 0, "LEFT_SPEED": 0 } }!'
DELIV_NAV_SPD_2 = '{ "SEQ": 0, "DELIV_NAV": { "RIGHT_DIR": 1, "LEFT_DIR": 1, "RIGHT_SPEED": 1, "LEFT_SPEED": 1 } }!'

msg_list = [ZONE1_DEFAULT_1, ZONE1_DEFAULT_2, ZONE2_DEFAULT_1, ZONE2_DEFAULT_2, ZONE3_DEFAULT_1, 
	ZONE3_DEFAULT_2, DELIV_NAV_STATUS_1, DELIV_NAV_STATUS_2, DELIV_NAV_POS_1, DELIV_NAV_POS_2, DELIV_NAV_SPD_1, DELIV_NAV_SPD_2 ]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))

file.write("Connected to server: \n")

count = 1
for msg in msg_list:
	print(msg + " : " + str(count))
	count = count + 1

while True:
	msg_num = int(input("Message to send to server: "))
	msg_num = msg_num - 1
	json_str = msg_list[msg_num]
	file.write("Sending to server: \n")
	s.send(json_str.encode())
	data = s.recv(40)
	print("Message received from server: ", data)
file.close()