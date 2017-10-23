# Function library for handling messages that are sent to the server, 
# performing database calls, and standard error reporting  
import json

# Server globals
# Message constants
SEQ_FIELD = "SEQ" 
WIFLY_INIT_MSG = '*HELLO*'

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

# Determines if a string is JSON format
def isjson(json_str):
    try:
        json_obj = json.loads(json_str)
    except ValueError:
        return False
    return True

