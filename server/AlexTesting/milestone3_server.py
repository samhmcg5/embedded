import socket
import sys

host = '192.168.1.123'
port = 2002
backlog = 5
length = None
size = 1
buffer = ""
data = ""

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)

print("Waiting for a connection:")
sys.stdout.flush()
client, address = s.accept()
print("Connected to WiFly:")
sys.stdout.flush()
#s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# msg = input("Enter Message: ")
print("Sending scan command...")
sys.stdout.flush()
#client.send(msg.encode())

#enable sensor simulation
client.send("{ \"seq\":0, \"sensor\": 1 }".encode())

i = 0
while i < 2:
    client.send("{ \"seq\":0, \"act\":0, \"dist\":50, \"speed\":5}".encode())
    client.send("{ \"seq\":0, \"act\":1, \"dist\":50, \"speed\":5}".encode())
    i += 1

while True:
    
    
    while data != '!':
        data = client.recv(size).decode()
        buffer += data
    print(buffer)
    sys.stdout.flush()
    #if "STATUS" in buffer:
    #print(buffer)
    #buffer = buffer[:-1]
    buffer = ""
    data = ""
    
client.close()