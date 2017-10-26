import socket
import time
import sys
import json
import re

#fin = "input.txt"

# start the stopwatch
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
PORT    = 2002
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
sys.stdout.flush()
client, address = s.accept()
print("Connected to WiFly")
sys.stdout.flush()

# send the instructions to the client
try:
    print("Sending instructions...")
    sys.stdout.flush()
    # f = open(fin,'r')
    # for line in f:
    #     if not '#' in line:
    client.send("{ \"seq\":0, \"act\":0, \"dist\":20, \"speed\":5}".encode())
    #f.close()
except:
    sys.exit("Failed to parse input file, exiting")
    sys.stdout.flush()

# listen for updates
while time.clock() <= 45.0:
    while data != '!':
        data = client.recv(SIZE).decode()
        buf += data

    buf = buf[:-1]
    if "*HELLO*" in buf:
        buf = re.sub("\*HELLO\*","" , buf)

    try:
        json_obj = json.loads(buf)

        if "STATUS" in buf:
            IDlE_TIME = time.time()
            diff = IDlE_TIME- PREV_IDLE_TIME
            print("%f\t%s" % (diff, buf))
            sys.stdout.flush()
            PREV_IDLE_TIME = IDlE_TIME
            total_idle += diff
            num_idle += 1
        elif "DIST" in buf:
            LOC_TIME = time.time()
            diff = LOC_TIME - PREV_LOC_TIME
            print("%f\t%s" % (diff, buf))
            sys.stdout.flush()
            PREV_LOC_TIME = LOC_TIME
            total_loc += diff
            num_loc += 1
        elif "ERROR" in buf:
            print("%d\t\t%s" %(errors, buf))
            sys.stdout.flush()
            errors += 1
        else:
            print(buf)
            sys.stdout.flush()
    except ValueError:
        print("Unable to parse JSON")
        sys.stdout.flush()

    data = ""
    buf  = ""

client.close()


print()
sys.stdout.flush()
print("STATISTICS:")
sys.stdout.flush()
avg_idle = total_idle / num_idle
avg_loc  = total_loc / num_loc
print("Average time per STATUS message = %f" % avg_idle)
sys.stdout.flush()
print("Average time per LOCATION message = %f" % avg_loc)
sys.stdout.flush()
print("ERROR messages received = %d" % errors)
sys.stdout.flush()