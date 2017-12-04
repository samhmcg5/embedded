import socket
import time

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

f = open("input.txt",'r')

print("Waiting for a connection:")
client, address = s.accept()
print("Connected to WiFly:")

while True:
    #for line in f:
    #    if not '#' in line:
    #       client.send(line.encode())

    msg = input("Enter Message: ")
    client.send(str(msg).encode())

    while data != '!':
        data = client.recv(size).decode()
        buffer += data
    #if "MAGNET" in buffer:
    print(buffer)
    buffer = ""
    data = ""

client.close()
# { "seq":0, "act":0, "dist":50, "speed":3}
# { "seq":0, "act":3, "dist":1284, "speed":3}
