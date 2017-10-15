import socket
import time
import sys
import json

#fin = "input.txt"

time.clock()

# time in s between messages
LOCATION = 1
IDLE     = 2

LOC_TIME       = 0.0
PREV_LOC_TIME  = time.time()
IDlE_TIME      = 0.0
PREV_IDLE_TIME = time.time()

total_idle = 0
total_loc  = 0
num_idle   = 0
num_loc    = 0
errors     = 0

# server definitions
HOST    = '192.168.1.123'
PORT    = 2000
BACKLOG = 5
SIZE    = 1

# used to recv
buf  = ""
data = ""

# instantiate the socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST,PORT))
s.listen(BACKLOG)

# connect to the Pic (or a mock client)
print("Waiting for a connection:")
client, address = s.accept()
print("Connected to WiFly")

# send the instructions to the client
try:
    print("Sending instructions...")
    # f = open(fin,'r')
    # for line in f:
    #     if not '#' in line:
    client.send("{ \"seq\":0, \"act\":0, \"dist\":20, \"speed\":5}".encode())
    #f.close()
except:
    sys.exit("Failed to parse input file, exiting")

# listen for updates
while time.clock() <= 45.0:
    while data != '!':
        data = client.recv(SIZE).decode()
        buf += data

    buf = buf[:-1]

    if "STATUS" in buf:
        IDlE_TIME = time.time()
        diff = IDlE_TIME- PREV_IDLE_TIME
        print("%f\t%s" % (diff, buf))
        PREV_IDLE_TIME = IDlE_TIME
        total_idle += diff
        num_idle += 1
    elif "X" in buf:
        LOC_TIME = time.time()
        diff = LOC_TIME - PREV_LOC_TIME
        print("%f\t%s" % (diff, buf))
        PREV_LOC_TIME = LOC_TIME
        total_loc += diff
        num_loc += 1
    elif "ERROR" in buf:
        errors += 1
    else:
        print(buf)

    data = ""
    buf  = ""

client.close()


print()
print("STATISTICS:")
avg_idle = total_idle / num_idle
avg_loc  = total_loc / num_loc
print("Average time per STATUS message = %f" % avg_idle)
print("Average time per LOCATION message = %f" % avg_loc)
print("ERROR messages received = %f" % errors)
