import socket
import time
import json
import re

host = '192.168.1.123'
port = 2000
backlog = 5
length = None
size = 1
buffer = ""
data = ""

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)

print("Waiting for a connection:")
client, address = s.accept()
print("Connected to WiFly:")

client.send( "{\"seq\":0, \"COLOR\":1, \"ZONE\":1}".encode() )
client.send( "{\"seq\":0, \"MAGNET\":1, \"IR\":2}".encode() )

while True:
    while data != '!':
        data = client.recv(size).decode()
        buffer += data
    buffer = buffer[:-1]
    if "*HELLO*" in buffer:
        buffer = re.sub("\*HELLO\*","" , buffer)
    # if "-->" in buffer:
    print(buffer)
    buffer = ""
    data = ""

client.close()
