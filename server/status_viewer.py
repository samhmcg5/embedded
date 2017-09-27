import socket
import json
import sys
import signal
from colorama import init
init()
from colorama import Back

#################################
########### GLOBALS #############
#################################

host    = '192.168.1.124'
port    = 2004
backlog = 5
length  = None
size    = 1
buf     = ""
data    = ""
client  = None

#################################
####### HELPER FUNCTIONS ########
#################################

# Handle a SIGINT
def signalHandler(signal, frame):
    print('Exiting...')
    sys.stdout.flush()
    if client is not None:
        print('Closing port...')
        sys.stdout.flush()
        client.close()
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
    color = getColor(json_obj['type'])
    print(color + "[%s-%s] %s" % (json_obj['pic'], json_obj['type'],json_obj['message']))
    sys.stdout.flush()

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
    s.bind((host,port))
    s.listen(backlog)
    return s

# read what should be an entire JSON message from the socket
def readMessage(cli):
    buf = ""
    data = ""
    while data != '!':
        data = cli.recv(size).decode()
        buf += data
    # remove the delimeter
    buf  = buf[:-1]
    return buf



#################################
######## Main Function ##########
#################################

def main():
    sock = createSocket()
    print("Waiting for connection to server...")
    sys.stdout.flush()
    client, address = sock.accept()
    print("Connected to serber")
    sys.stdout.flush()
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    while True:
        buf = readMessage(client)
        if isJSON(buf):
            json_obj = json.loads(buf)
            writeJSON(json_obj)
        else:
            print("INCOMING DATA ERRPR: Not JSON Type")
            sys.stdout.flush()
            continue


main()
client.close()