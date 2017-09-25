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
SERVER_ONLINE = False

# Database globals
localhost = '127.0.0.1'
mongo_port = 27017
mongo_client = None
MONGODB_ONLINE = False
DATABASE_NAME = 'TEAM14'
SCANNER_SENSING_COL_NAME = 'SCANNER_SENSING'
db = None
scan_snsg_col = None

# Message constants
DELIM = "!"
SEQ_FIELD = "SEQ" 
WIFLY_INIT_MSG = '*HELLO*'
RECV = 'RECV'
SENT = 'SENT'
FORMAT_ERR = 'FORMAT_ERR'
SCAN_SENSE = 'SCAN_SENSE'

# Scanner sensing fields
RED = 'RED'
BLUE = 'BLUE'
GREEN = 'GREEN'

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
INFO_SRV_MSG_SENT = 'SERVER INFO: Sent message to client.'

# Standard error messages
ERROR_SRV_CONN = 'SERVER ERROR: Client is not connected! Attempting to reconnect...'
ERROR_MSG_FORMAT = 'SERVER ERROR: Message is not JSON object type!'
ERROR_DB_CONN = 'DATABASE ERROR: Database is not connected!'


# Updates incoming messaage sequence number to expected result 
def handle_seq(json_obj):
	print(json_obj[SEQ_FIELD])
	sys.stdout.flush()
	seq_num = json_obj[SEQ_FIELD] + 1
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
def store(criteria, json_obj, mongo):
	#if MONGODB_ONLINE is True:
		print(INFO_DB_STORE_ATT + " Data being stored: " + json.dumps(json_obj) + "\n")
		sys.stdout.flush()
		mongo[DATABASE_NAME][SCANNER_SENSING_COL_NAME].replaceOne(criteria, json_obj, "{ upsert: true }")
		print(INFO_DB_STORE_SUC)
		sys.stdout.flush()
	#else:
		raise ValueError(ERROR_DB_CONN + "\n")
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
	#db.create_collection(SCANNER_SENSING_COL_NAME)
	scan_snsg_col = db[SCANNER_SENSING_COL_NAME]
	scan_snsg_col.insert_one(json.loads('{ "COLLECTION_INIT": 1 }'))
	return mongo_client

# Determines if connected to database
def is_db_online():
	return MONGODB_ONLINE

# Determine if server is up and waiting for a connection
def is_srv_online():
	return SERVER_ONLINE

# Initialize and bring up server waiting for client connection
def start_server(ip_addr, port):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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
	if length is 0:
		print(INFO_SRV_MSG_WAIT + "\n")
		sys.stdout.flush()
	data = None
	try: 
		data = client.recv(size)
	except (ConnectionError, KeyboardInterrupt):
		SERVER_ONLINE = False
		print(ERROR_SRV_CONN + "\n")
		sys.stdout.flush()
		return
	data = data.decode()	
	return data 

# Determines if buffer contains wifly init msg
def init_msg(buffer):
	return buffer == WIFLY_INIT_MSG

# Returns message constants
def get_msg_constants():
	return DELIM, SEQ_FIELD, RECV, SENT, FORMAT_ERR

# Returns message field names for various rover responsibilities
"""
ADD FIELD FOR YOUR ROVER TASK (SCAN_NAV, DELIV_NAV, etc)

Intended to be used as the top field for message 
"""
def get_rover_msg_fields():
	return SCAN_SENSE

# Return the name of the collections
def get_collections():
	return scan_snsg_col

def get_scanner_rover_fields():
	return RED, BLUE, GREEN

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