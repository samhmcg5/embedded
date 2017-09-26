import socket
import json
import sys
import signal

client = None

def signal_handler(signal, frame):
        print('Exiting...')
        sys.stdout.flush()
        if client is not None:
            print('Closing port...')
            sys.stdout.flush()
            client.close()
        sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)


# JSON format === { "pic":"str", "type":"str", "message":"str" }
# type = 0 for update
#      = 1 for error
#      = 2 for request

# Determines if an incoming message is in JSON format
ERROR_MSG_FORMAT = 'SERVER ERROR: Message is not JSON object type!'
def is_json(json_str):
    try:
        json_obj = json.loads(json_str)
    except ValueError:
        print(ERROR_MSG_FORMAT + "\n")
        sys.stdout.flush()
        return False
    return True


host    = '192.168.1.123'
port    = 2005
backlog = 5
length  = None
size    = 1
buf     = ""
data    = ""

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)

print("Waiting for a connection:")
sys.stdout.flush()
client, address = s.accept()
print("Connected to Client:")
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
        print("[%s-%s] %s" % (json_obj['pic'], json_obj['type'],json_obj['message']))
        sys.stdout.flush()
    else:
        buf  = ""
        data = ""
        continue    
    buf  = ""
    data = ""

client.close()