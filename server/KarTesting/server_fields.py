# Server fields and globals
delim = '!'

# Server connection fields
IP_ADDR = '192.168.1.124' 
deliv_nav_port 		= 2000
deliv_sense_port 	= 2001
scan_nav_port 		= 2002
scan_sense_port 	= 2003
status_port 		= 2004

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