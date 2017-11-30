from pymongo import MongoClient
import socket
import json
import sys

###############
### GLOBALS ###
###############


######################
### MONGO SETTINGS ###
######################
localhost = '127.0.0.1'
mongo_port = 27017
mongo_client = None
global MONGODB_ONLINE
MONGODB_ONLINE = False
DATABASE_NAME = 'team14'
SCANNER_SENSING_COL_NAME = 'SCANNER_SENSING'
SCANNER_NAVIGATION_COL_NAME = 'SCANNER_NAVIGATION'
DELIVERY_SENSING_COL_NAME = 'DELIVERY_SENSING'
DELIVERY_NAVIGATION_COL_NAME = 'DELIVERY_NAVIGATION'

#########################
### MESSAGE CONSTANTS ###
#########################
DELIM = "!"
SEQ_FIELD = "SEQ"
WIFLY_INIT_MSG = '*HELLO*'
RECV = 'RECV'
SENT = 'SENT'
FORMAT_ERR = 'FORMAT_ERR'


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

# Initiate connection to the database and initialize fields
def connect_to_mongo():
    print(INFO_DB_CONN_ATT)
    sys.stdout.flush()

    # Connect to Mongo
    mongo_client = MongoClient(localhost, mongo_port)
    # Connected, now initialize database names and collections with initial default messages, to create them
    db = mongo_client[DATABASE_NAME]
    scan_snsg_col = db[SCANNER_SENSING_COL_NAME]
    scan_nav_col = db[SCANNER_NAVIGATION_COL_NAME]
    deliv_snsg_col = db[DELIVERY_SENSING_COL_NAME]
    deliv_nav_col = db[DELIVERY_NAVIGATION_COL_NAME]
    create_default_server_msg_fields(scan_snsg_col, SCANNER_SENSING_COL_NAME)
    create_default_server_msg_fields(scan_nav_col, SCANNER_NAVIGATION_COL_NAME)
    create_default_server_msg_fields(deliv_snsg_col, DELIVERY_SENSING_COL_NAME)
    create_default_server_msg_fields(deliv_nav_col, DELIVERY_NAVIGATION_COL_NAME)
    return