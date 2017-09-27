import socket
import sys

host    = '192.168.1.123'
port    = int(sys.argv[1])
backlog = 5
length  = None
size    = 1024
buffer  = ""
data    = ""

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

print("Waiting for a connection:")
s.connect((host,port))
print("Connected to Status Viewer:")

while True:
    msg = input("Enter Message: ")
    s.send(msg.encode())
    
s.close()
