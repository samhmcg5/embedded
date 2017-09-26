import socket

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
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

while True:
    msg = input("Enter Message: ")
    client.send(msg.encode())
    
    while data != '!':
        data = client.recv(size).decode()
        buffer += data
    #if "STATUS" in buffer:
    #print(buffer)
    #buffer = buffer[:-1]
    buffer = ""
    data = ""
    
client.close()
