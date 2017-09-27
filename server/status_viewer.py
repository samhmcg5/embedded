import socket
import json
import sys
import signal
import defines_status_viewer as def_status
# to add some color
from colorama import init
init()
from colorama import Back

#################################
########### GLOBALS #############
#################################

host    = '192.168.1.123'
port    = 2004
backlog = 5
length  = 1024
size    = 1
buf     = ""
data    = ""
#client  = None

#################################
####### HELPER FUNCTIONS ########
#################################

# Handle a SIGINT
def signalHandler(signal, frame):
    print('Exiting...')
    sys.stdout.flush()
    # if client is not None:
    #     print('Closing port...')
    #     client.close()
    sys.exit(0)

# register the signal handler 
signal.signal(signal.SIGINT, signalHandler)

# Do we have a JSON object?
def isJSON(json_str):
    try:
        json_obj = json.loads(json_str)
    except ValueError:
        return False
    return True

# write the JSON to terminal and the correct file
def writeJSON(json_obj):
    color = getColor(json_obj['mtype'])
    print(color + "[%s-%s] %s" % (json_obj['origin'], json_obj['mtype'],json_obj['text']))

# get the color to show based on the type of message
def getColor(str):
    if str == 'ERROR':
        return Back.RED
    if str == 'REQUEST':
        return Back.GREEN
    else:
        return Back.RESET

# return a connection
def createSocket():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host,port))
    return s

# read what should be an entire JSON message from the socket
def readMessage(sock):
    buf = ""
    data = ""
    while data != '!':
        data = sock.recv(size).decode()
        buf += data
    # remove the delimeter
    buf  = buf[:-1]
    return buf



#################################
######## Main Function ##########
#################################

def main():
    print("Waiting for connection to server...")
    sock = createSocket()
    print("Connected to server")
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    while True:
        buf = readMessage(sock)
        print(buf)
        #print(buf)
        if isJSON(buf):
            json_obj = json.loads(buf)
            writeJSON(json_obj)
        else:
            print("INCOMING DATA ERROR: Not JSON Type")
            continue


main()
