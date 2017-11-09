import socket
import sys
import time

time.clock()

cm10  = ["{ \"seq\":0, \"act\":0, \"dist\":10, \"speed\":2}"]
cm25  = ["{ \"seq\":0, \"act\":0, \"dist\":15, \"speed\":3}"]
cm50f  = ["{ \"seq\":0, \"act\":0, \"dist\":15, \"speed\":4}",
         "{ \"seq\":0, \"act\":0, \"dist\":10, \"speed\":4}"]
cm50b  = ["{ \"seq\":0, \"act\":1, \"dist\":15, \"speed\":4}",
         "{ \"seq\":0, \"act\":1, \"dist\":10, \"speed\":4}"]
cm100f = ["{ \"seq\":0, \"act\":0, \"dist\":20, \"speed\":4}",
         "{ \"seq\":0, \"act\":0, \"dist\":20, \"speed\":4}",
         "{ \"seq\":0, \"act\":0, \"dist\":10, \"speed\":4}"]
cm100b = ["{ \"seq\":0, \"act\":1, \"dist\":20, \"speed\":4}",
         "{ \"seq\":0, \"act\":1, \"dist\":20, \"speed\":4}",
         "{ \"seq\":0, \"act\":1, \"dist\":10, \"speed\":4}"]
cm80  = ["{ \"seq\":0, \"act\":1, \"dist\":20, \"speed\":3}"]

# server definitions
HOST    = '192.168.1.123'
PORT    = 2002
BACKLOG = 5
SIZE    = 1

# instantiate the socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST,PORT))
s.listen(BACKLOG)

# connect to the Pic (or a mock client)
print("Waiting for a connection:")
sys.stdout.flush()
client, address = s.accept()
print("Connected to WiFly")
sys.stdout.flush()

def listenForIdle():
    buf = ""
    data = ""
    paused = True
    while True:
        while data != '!':
            data = client.recv(SIZE).decode()
            buf += data
        if "DIST" in buf:
            print(buf)
            sys.stdout.flush()
        if "IDLE" in buf and paused:
            pause_time = time.clock()
            paused = False
        buf = ""
        data = ""
        if not paused:
            if time.clock() - pause_time >= 7:
                return


# client.send(cm10[0].encode())
# listenForIdle()

# client.send(cm25[0].encode())
# listenForIdle()

client.send(cm50f[0].encode())
client.send(cm50f[1].encode())
listenForIdle()

client.send(cm50b[0].encode())
client.send(cm50b[1].encode())
listenForIdle()

client.send(cm100f[0].encode())
client.send(cm100f[1].encode())
client.send(cm100f[2].encode())
listenForIdle()

client.send(cm100b[0].encode())
client.send(cm100b[1].encode())
client.send(cm100b[2].encode())
listenForIdle()

# client.send(cm80[0].encode())
# listenForIdle()


client.close()