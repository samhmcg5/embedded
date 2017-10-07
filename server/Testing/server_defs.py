# Function library for handling messages that are sent to the server, 
# performing database calls, and standard error reporting  

import socket
import json
import sys
from threading import Thread

sys.stdout.flush()
# Server globals
backlog = 4
size = 1
global SERVER_ONLINE
SERVER_ONLINE = False

# Message constants
DELIM = "!"
SEQ_FIELD = "SEQ" 
WIFLY_INIT_MSG = '*HELLO*'
RECV = 'RECV'
SENT = 'SENT'
FORMAT_ERR = 'FORMAT_ERR'

# Scanner sensing fields
SCAN_SENSE = 'SCAN_SENSE'
ZONE = 'ZONE'
RED = 'RED'
BLUE = 'BLUE'
GREEN = 'GREEN'

# Scanner navigation fields
SCAN_NAV = 'SCAN_NAV'
DISTANCE = 'DISTANCE'

# Delivery navigation fields
DELIV_NAV = 'DELIV_NAV'
STATUS = 'STATUS' 
MESSAGE = 'MESSAGE'
X = 'X' 
Y = 'Y' 
RIGHT_DIR = 'RIGHT_DIR' 
LEFT_DIR = 'LEFT_DIR' 
RIGHT_SPEED = 'RIGHT_SPEED' 
LEFT_SPEED = 'LEFT_SPEED'

DELIV_SENSE = 'DELIV_SENSE'

# Info messages
INFO_SRV_START          = 'Starting server.'
INFO_SRV_WAIT           = 'Waiting for client connection.'
INFO_SRV_CONNECT        = 'Connected to client.'
INFO_SRV_MSG_WAIT       = 'Waiting for msg...'
INFO_SRV_MSG_RECV       = 'Received msg: '
INFO_SRV_MSG_SENDING    = 'Sending message to client.'
INFO_SRV_MSG_SENT       = 'Sent message to client.'

# Standard error messages
ERROR_SRV_CONN          = 'Client is not connected, Attempting to reconnect...'
ERROR_MSG_FORMAT        = 'Message is not JSON object type'

# Updates incoming messaage sequence number to expected result 
def handle_seq(seq):
    seq_num = seq + 1
    return seq_num;

# Determines if a string is JSON format
def isjson(json_str):
    try:
        json_obj = json.loads(json_str)
    except ValueError:
        return False
    return True

# Determine if server is up and waiting for a connection
def is_srv_online():
    return SERVER_ONLINE

# Initialize and bring up server waiting for client connection
def start_server(ip_addr, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((ip_addr, port))
    s.listen(backlog)
    #print(INFO_SRV_START + "\n")
    sys.stdout.flush()
    return s

# Wait for client to connect to server
def client_connect(sckt):
    #print(INFO_SRV_WAIT)
    sys.stdout.flush()
    client, address = sckt.accept()
    SERVER_ONLINE = True
    #print(INFO_SRV_CONNECT + " Client Address: " + str(address) + "\n")
    return client, address

# Receive single byte from client
def recv_msg(client, length):
    data = None
    try:
        data = client.recv(size)
    except (ConnectionError, KeyboardInterrupt):
        SERVER_ONLINE = False
        #print(ERROR_SRV_CONN + "\n")
        #sys.stdout.flush()
        return
    data = data.decode()    
    return data 

# Deliver message to client
def send_msg(client, msg):
    #print(INFO_SRV_MSG_SENDING)
    try:
        client.send(msg.encode())
        #print(INFO_SRV_MSG_SENT)
    except (ConnectionError): 
        SERVER_ONLINE = False
        #print(ERROR_SRV_CONN)
        #sys.stdout.flush()
    return

# Determines if buffer contains wifly init msg
def init_msg(buffer):
    return buffer == WIFLY_INIT_MSG

# Returns message constants
def get_msg_constants():
    return DELIM, SEQ_FIELD, RECV, SENT, FORMAT_ERR

# Output message of specific type
def print_msg(case, msg):
    if case == RECV:
        print(INFO_SRV_MSG_RECV + msg + "\n")
        sys.stdout.flush()
    elif case == SENT:
        print(INFO_SRV_MSG_SENT + " Message sent: " + msg + "\n")
        sys.stdout.flush()
    elif case == FORMAT_ERR:
        print(ERROR_MSG_FORMAT + " Error: " + msg + "\n")
        sys.stdout.flush()
    return

def get_msg(case, msg):
    if case == RECV:
        return INFO_SRV_MSG_RECV + msg
    elif case == SENT:
        return INFO_SRV_MSG_SENT + " Message sent: " + msg
    elif case == FORMAT_ERR:
        return ERROR_MSG_FORMAT + " Error: " + msg
    return
