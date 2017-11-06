import socket
import time
import json
import re
import sys

host = '192.168.1.123'
port = 2000
backlog = 5
length = None
size = 1
buffer = ""
data = ""

prev_mag_state = 0
next_mag_state = 0
seq = 1
color = 0
zone = 1

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)

print("Waiting for a connection:")
client, address = s.accept()
print("Connected to WiFly:")

client.send( "{\"seq\":0, \"COLOR\":0, \"ZONE\":0}".encode() )
client.send( "{\"seq\":1, \"MAGNET\":0, \"IR\":7}".encode() )

while True:
    while data != '!':
        data = client.recv(size).decode()
        buffer += data
    buffer = buffer[:-1]
    if "*HELLO*" in buffer:
        buffer = re.sub("\*HELLO\*","" , buffer)
    json_obj = json.loads(buffer)
    print(json_obj)
    buffer = ""
    data = ""

    if "SET_MAGNET" in json_obj["DELIV_NAV"].keys():
        if json_obj['DELIV_NAV']['SET_MAGNET'] != prev_mag_state:
            next_mag_state = json_obj['DELIV_NAV']['SET_MAGNET']
            prev_mag_state = json_obj['DELIV_NAV']['SET_MAGNET']
            print("--> setting MAGNET")
        msg = "{\"seq\":%i, \"MAGNET\":%i, \"IR\":4}" % (seq, next_mag_state)
        seq += 1
        prev_mag_state = json_obj['DELIV_NAV']["SET_MAGNET"]
        client.send( msg.encode() )

    if 'STATUS' in json_obj["DELIV_NAV"].keys():
        if (color >= 3):
            sys.exit("--> Done...")
        if "IDLE" in json_obj['DELIV_NAV']['MSG']:
            print("--> Sending new task...")
            msg = "{\"seq\":%i, \"COLOR\":%i, \"ZONE\":%i}" % (seq,color,zone) 
            seq += 1
            color += 1
            zone  += 1
            if zone == 3:
                zone = 0
            client.send( msg.encode() )

client.close()
