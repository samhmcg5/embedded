import socket

host = '192.168.1.123'
port = 2000
size = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))

print("Connected to server: \n")

A = '{ "SEQ": 0, "DELIV_NAV": { "SET_MAGNET":0, "IR_DATA":0 } }!'
B = '{ "SEQ": 0, "DELIV_NAV": { "SET_MAGNET":1, "IR_DATA":1 } }!'

MSGS = [A, B]

while True:
    msg_num = int(input("Message to send to server: "))
    json_str = MSGS[msg_num]
    s.send(json_str.encode())
