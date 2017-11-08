from course_defines import *

import socket
import sys
import time

time.clock()

# server definitions
HOST    = '192.168.1.123'
PORT    = 2000
BACKLOG = 5
SIZE    = 1

# instantiate the socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST,PORT))
s.listen(BACKLOG)

# connect to the Pic (or a mock client)
print("Waiting for a connection:")
client, address = s.accept()
print("Connected to WiFly")

def listenForIdle():
    buf = ""
    data = ""
    paused = True
    while True:
        while data != '!':
            data = client.recv(SIZE).decode()
            buf += data
        if "X" in buf:
            print(buf)
        if "IDLE" in buf and paused:
            pause_time = time.clock()
            paused = False
        buf = ""
        data = ""
        if not paused:
            if time.clock() - pause_time >=2:
                return

def sendActions(actions):
    for act in actions:
        client.send(act.encode())

# Execute

sendActions(check1)
listenForIdle()

sendActions(check2)
listenForIdle()

sendActions(check3)
listenForIdle()

sendActions(check4)
listenForIdle()

sendActions(check5)
listenForIdle()

client.close()
