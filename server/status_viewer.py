import socket
import json
import sys
import signal
from colorama import init
init()
from colorama import Back

client = None

# JSON format === { "pic":"str", "type":"str", "message":"str" }
# type = 0 for update
#      = 1 for error
#      = 2 for request

def signal_handler(signal, frame):
        println('Exiting...')
        sys.stdout.flush()
        if client is not None:
            println('Closing port...')
            sys.stdout.flush()
            client.close()
        sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

# Determines if an incoming message is in JSON format
ERROR_MSG_FORMAT = 'SERVER ERROR: Message is not JSON object type!'
def is_json(json_str):
    try:
        json_obj = json.loads(json_str)
    except ValueError:
        println(ERROR_MSG_FORMAT + "\n")
        sys.stdout.flush()
        return False
    return True

#default color to regular
def println(line, color=""):
    print(color + line)
    sys.stdout.flush()

def getColor(str):
    if str == 'ERROR':
        return Back.RED
    if str == 'REQUEST':
        return Back.GREEN
    else:
        return Back.RESET

def main():
    host    = '192.168.1.124'
    port    = 2005
    backlog = 5
    length  = None
    size    = 1
    buf     = ""
    data    = ""

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host,port))
    s.listen(backlog)

    println("Waiting for a connection:")
    sys.stdout.flush()
    client, address = s.accept()
    println("Connected to Client:")
    sys.stdout.flush()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    while True:
        # read data from the server 
        while data != '!':
            data = client.recv(size).decode()
            buf += data
        # parse the buffer into json
        buf  = buf[:-1]
        if is_json(buf) is True:
            json_obj = json.loads(buf)
            color = getColor(json_obj['type'])
            println(color + "[%s-%s] %s" % (json_obj['pic'], json_obj['type'],json_obj['message']))
            sys.stdout.flush()
        else:
            buf  = ""
            data = ""
            continue    
        buf  = ""
        data = ""
    client.close()

if __name__ == '__main__':
    main()