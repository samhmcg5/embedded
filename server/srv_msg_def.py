# Function library for handling messages that are sent to the server, 
# performing database calls, and standard error reporting  

from pymongo import MongoClient
import socket
import json
import sys
sys.stdout.flush()
# Server globals
backlog = 4
size = 1
global SERVER_ONLINE
SERVER_ONLINE = False

# Database globals
localhost = '127.0.0.1'
mongo_port = 27017
mongo_client = None
global MONGODB_ONLINE
MONGODB_ONLINE = False
DATABASE_NAME = 'TEAM14'
SCANNER_SENSING_COL_NAME = 'SCANNER_SENSING'
SCANNER_NAVIGATION_COL_NAME = 'SCANNER_NAVIGATION'
DELIVERY_SENSING_COL_NAME = 'DELIVERY_SENSING'
DELIVERY_NAVIGATION_COL_NAME = 'DELIVERY_NAVIGATION'
db = None

# Database collections
scan_snsg_col = None
scan_nav_col = None
deliv_snsg_col = None
deliv_nav_col = None

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
SCAN_SENSE_DEFAULT_ZONE_ACTION = '{ "SEQ": 0, "ZONE": 1, "ACTION": 0 }' 
SCAN_SENSE_DEFAULT_ZONE_1 = '{ "SEQ": 0, "ZONE": 1, "RED": 0, "GREEN": 0, "BLUE": 0 }'
SCAN_SENSE_DEFAULT_ZONE_2 = '{ "SEQ": 0, "ZONE": 2, "RED": 0, "GREEN": 0, "BLUE": 0 }'
SCAN_SENSE_DEFAULT_ZONE_3 = '{ "SEQ": 0, "ZONE": 3, "RED": 0, "GREEN": 0, "BLUE": 0 }'

# Scanner navigation fields
SCAN_NAV = 'SCAN_NAV'
DISTANCE = 'DISTANCE'
SCAN_NAV_DEFAULT_ACTION = '{ "SEQ": 0, "ACTION": 0 }'

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
DELIV_NAV_DEFAULT_PATH = '{ "SEQ": 0, "COLOR": 0, "X": 0}'
DELIV_NAV_DEFAULT_ACTION = '{ "SEQ": 0, "ACTION": 0, "DISTANCE": 0, "INTENSITY":0 }'

# Delivery Sensing fields
DELIV_SENSE = 'DELIV_SENSE'
DELIV_SENSE_DEFAULT_ACTION = '{ "SEQ": 0, "ACTION": 0 }'
# ZONE and STATUS defined same as before

# Info messages
INFO_DB_CONN_ATT = 'DATABASE INFO: Attempting to connect to database.' 
INFO_DB_CONN = 'DATABASE INFO: Connected to database.'
INFO_DB_INIT = 'DATABASE INFO: Initializing database fields.'
INFO_DB_INIT_SENSING = 'DATABASE INFO: Initializing sensing scanner rover collection.'
INFO_DB_STORE_ATT = 'DATABASE INFO: Attempting to store and update data.'
INFO_DB_STORE_SUC = 'DATABASE INFO: Stored data successfully.'
INFO_SRV_START = 'SERVER INFO: Starting server.'
INFO_SRV_WAIT = 'SERVER INFO: Waiting for client connection.'
INFO_SRV_CONNECT = 'SERVER INFO: Connected to client.'
INFO_SRV_MSG_WAIT = 'SERVER INFO: Waiting for message from client.'
INFO_SRV_MSG_RECV = 'SERVER INFO: Received message from client.'
INFO_SRV_MSG_SENDING = 'SERVER INFO: Sending message to client.'
INFO_SRV_MSG_SENT = 'SERVER_INFO: Sent message to client.'

# Standard error messages
ERROR_SRV_CONN = 'SERVER ERROR: Client is not connected! Attempting to reconnect...'
ERROR_MSG_FORMAT = 'SERVER ERROR: Message is not JSON object type!'
ERROR_DB_CONN = 'DATABASE ERROR: Database is not connected!'

# Creates default scanner sensing db fields
def create_default_server_msg_fields(col, name):
	list = []
	json_obj = None
	if name is SCANNER_SENSING_COL_NAME:
		json_obj = json.loads(SCAN_SENSE_DEFAULT_ZONE_ACTION)
		list.append(json_obj)
		json_obj = json.loads(SCAN_SENSE_DEFAULT_ZONE_1)
		list.append(json_obj)
		json_obj = json.loads(SCAN_SENSE_DEFAULT_ZONE_2)
		list.append(json_obj)
		json_obj = json.loads(SCAN_SENSE_DEFAULT_ZONE_3)
		list.append(json_obj)
	elif name is SCANNER_NAVIGATION_COL_NAME:
		json_obj = json.loads(SCAN_NAV_DEFAULT_ACTION)
		list.append(json_obj)
	elif name is DELIVERY_SENSING_COL_NAME:
		json_obj = json.loads(DELIV_SENSE_DEFAULT_ACTION)
		list.append(json_obj)
	elif name is DELIVERY_NAVIGATION_COL_NAME:
		json_obj = json.loads(DELIV_NAV_DEFAULT_PATH)
		list.append(json_obj)
		json_obj = json.loads(DELIV_NAV_DEFAULT_ACTION)
		list.append(json_obj)
	
	for json_obj in list:
		col.replace_one(json_obj, json_obj, True)
	return

# Updates incoming messaage sequence number to expected result 
def handle_seq(seq):
	seq_num = seq + 1
	return seq_num;

# Determines if an incoming message is in JSON format
def is_json(json_str):
	try:
		json_obj = json.loads(json_str)
	except ValueError:
		print(ERROR_MSG_FORMAT + "\n")
		sys.stdout.flush()
		return False
	return True

# Stores a JSON formatted object in the database
def store(criteria, json_obj, col):
	print(INFO_DB_STORE_ATT + " Data being stored: " + json.dumps(json_obj) + "\n")
	sys.stdout.flush()
	res = col.replace_one(criteria, json_obj, True)
	print(INFO_DB_STORE_SUC)
	sys.stdout.flush()
	return

# Gets action message to respective pic
def retrieve(seq_num, col):
	raw_doc = col.find_one('{ "ACTION": { "$exists": true } }')
	del raw_doc['_id']
	raw_doc[SEQ_FIELD] = seq_num
	return json.dumps(raw_doc) + DELIM


# Clean database
def clean_db():
	mongo_client = MongoClient()
	mongo_client.drop_database(DATABASE_NAME)
	print(INFO_DB_INIT + "\n")
	sys.stdout.flush()
	db = mongo_client[DATABASE_NAME]
	print(INFO_DB_INIT_SENSING + "\n")
	sys.stdout.flush()
	scan_snsg_col = db[SCANNER_SENSING_COL_NAME]
	scan_nav_col = db[SCANNER_NAVIGATION_COL_NAME]
	deliv_snsg_col = db[DELIVERY_SENSING_COL_NAME]
	deliv_nav_col = db[DELIVERY_NAVIGATION_COL_NAME]
	create_default_server_msg_fields(scan_snsg_col, SCANNER_SENSING_COL_NAME)
	create_default_server_msg_fields(scan_nav_col, SCANNER_NAVIGATION_COL_NAME)
	create_default_server_msg_fields(deliv_snsg_col, DELIVERY_SENSING_COL_NAME)
	create_default_server_msg_fields(deliv_nav_col, DELIVERY_NAVIGATION_COL_NAME)
	return

# Initiate connection to the database and initialize fields
def connect_to_mongo():
	print(INFO_DB_CONN_ATT + "\n")
	sys.stdout.flush()

	# Connect to Mongo
	mongo_client = MongoClient(localhost, mongo_port)
	MONGODB_ONLINE = True
	print(INFO_DB_CONN + "\n")
	sys.stdout.flush()
	
	# Connected, now initialize database names and collections with initial default messages, to create them
	print(INFO_DB_INIT + "\n")
	sys.stdout.flush()
	db = mongo_client[DATABASE_NAME]
	print(INFO_DB_INIT_SENSING + "\n")
	sys.stdout.flush()
	scan_snsg_col = db[SCANNER_SENSING_COL_NAME]
	scan_nav_col = db[SCANNER_NAVIGATION_COL_NAME]
	deliv_snsg_col = db[DELIVERY_SENSING_COL_NAME]
	deliv_nav_col = db[DELIVERY_NAVIGATION_COL_NAME]
	create_default_server_msg_fields(scan_snsg_col, SCANNER_SENSING_COL_NAME)
	create_default_server_msg_fields(scan_nav_col, SCANNER_NAVIGATION_COL_NAME)
	create_default_server_msg_fields(deliv_snsg_col, DELIVERY_SENSING_COL_NAME)
	create_default_server_msg_fields(deliv_nav_col, DELIVERY_NAVIGATION_COL_NAME)
	return

# Determines if connected to database
def is_db_online():
	return MONGODB_ONLINE

# Determine if server is up and waiting for a connection
def is_srv_online():
	return SERVER_ONLINE

# Initialize and bring up server waiting for client connection
def start_server(ip_addr, port):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.bind((ip_addr, port))
	s.listen(backlog)
	print(INFO_SRV_START + "\n")
	sys.stdout.flush()
	return s

# Wait for client to connect to server
def client_connect(sckt):
	print(INFO_SRV_WAIT + "\n")
	sys.stdout.flush()
	client, address = sckt.accept()
	SERVER_ONLINE = True
	print(INFO_SRV_CONNECT + " Client Address: " + str(address) + "\n")
	sys.stdout.flush()
	return client, address

# Receive single byte from client
def recv_msg(client, length):
	data = None
	try:
		if length is 0:
			print(INFO_SRV_MSG_WAIT + "\n")
			sys.stdout.flush()
		data = client.recv(size)
	except (ConnectionError, KeyboardInterrupt):
		SERVER_ONLINE = False
		print(ERROR_SRV_CONN + "\n")
		sys.stdout.flush()
		return
	data = data.decode()	
	return data 

# Deliver message to client
def send_msg(client, msg):
	print(INFO_SRV_MSG_SENDING + "\n")
	try:
		client.send(msg.encode())
		print(INFO_SRV_MSG_SENT + "\n")
	except (ConnectionError): 
		SERVER_ONLINE = False
		print(ERROR_SRV_CONN + "\n")
		sys.stdout.flush()
	return

# Determines if buffer contains wifly init msg
def init_msg(buffer):
	return buffer == WIFLY_INIT_MSG

# Returns message constants
def get_msg_constants():
	return DELIM, SEQ_FIELD, RECV, SENT, FORMAT_ERR

# Return the name of the collections
def get_collections():
	mongo_client = MongoClient()
	scan_snsg_col = mongo_client[DATABASE_NAME][SCANNER_SENSING_COL_NAME]
	scan_nav_col = mongo_client[DATABASE_NAME][SCANNER_NAVIGATION_COL_NAME]
	deliv_snsg_col = mongo_client[DATABASE_NAME][DELIVERY_SENSING_COL_NAME]
	deliv_nav_col = mongo_client[DATABASE_NAME][DELIVERY_NAVIGATION_COL_NAME]
	return scan_snsg_col, scan_nav_col, deliv_snsg_col, deliv_nav_col

# Return the json fields for a scanner sensing message
def get_scanner_sensing_fields():
	return SCAN_SENSE, ZONE, RED, BLUE, GREEN
	
# Return the json fields for a scanner navigation message
def get_scanner_navigation_fields():
	return SCAN_NAV, DISTANCE

# Return the json fields for a delivery navigation message
def get_delivery_navigation_fields():
	return DELIV_NAV, STATUS, MESSAGE, X, Y, RIGHT_DIR, LEFT_DIR, RIGHT_SPEED, LEFT_SPEED

# Return the json fields for a delivery sensing message
def get_delivery_sensing_fields():
	return DELIV_SENSE #, ZONE, STATUS (uses zone and status from deliv_nav fields since they are the same)

# Output message of specific type
def print_msg(case, msg):
	if case == RECV:
		print(INFO_SRV_MSG_RECV + " Message received: " + msg + "\n")
		sys.stdout.flush()
	elif case == SENT:
		print(INFO_SRV_MSG_SENT + " Message sent: " + msg + "\n")
		sys.stdout.flush()
	elif case == FORMAT_ERR:
		print(ERROR_MSG_FORMAT + " Error: " + msg + "\n")
		sys.stdout.flush()
	return