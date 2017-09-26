#!/usr/bin/env python3

"""
A simple echo server that handles some exceptions
"""
from pymongo import MongoClient
import bsonjs
import pprint as pprint
import srv_msg_def as srv
import sys
import socket
import json

# Server configurations
IP_ADDR = '192.168.1.123'
PORT = 20000

# Message data fields
DELIM, SEQ_FIELD, RECV, SENT, FORMAT_ERR = srv.get_msg_constants()
SCAN_NAV, DISTANCE = srv.get_scanner_navigation_fields()
SCAN_SENSE, ZONE, RED, BLUE, GREEN = srv.get_scanner_sensing_fields()
DELIV_NAV, STATUS, MESSAGE, X, Y, RIGHT_DIR, LEFT_DIR, RIGHT_SPEED, LEFT_SPEED = srv.get_delivery_navigation_fields()
DELIV_SENSE = srv.get_delivery_sensing_fields()

# Database collecitions

# Initialize and connect to database 
mongo = None
if srv.is_db_online() is not True:	
    mongo = srv.connect_to_mongo()
scan_snsg_col, scan_nav_col, deliv_snsg_col, deliv_nav_col  = srv.get_collections()

global s
global client
global address
global msg_length

s = None
client = None
address = None
msg_length = None
    
# Main server processing
while True:
    # Initialize and start server listening for clients
    buf = ""
    if srv.is_srv_online() is not True:  
    	s = srv.start_server(IP_ADDR, PORT)
    	client, address = srv.client_connect(s)
    
    # Handle messages
    while True:
        try:
            # Receive data byte by byte and add to buffer
            msg_length = len(buf)
            data = srv.recv_msg(client, msg_length)
            if not data:
           	    break
            buf += data
            while True:
                if srv.init_msg(buf) is True:
                    buf = ""
                    break
                if DELIM not in buf: # dont begin handling until delimeter recognized
                    break

                srv.print_msg(RECV, buf)
                buf = buf[:-1] # remove delimeter to create JSON object
                json_obj = None
                
                if srv.is_json(buf) is True:
                    json_obj = json.loads(buf)
                else:
                    buf = ""
                    data = ""
                    break
                buf = ""
                data = "" 

                # Begin to handle messages
                scan_sense_rtrn_msg = ""  
                scan_nav_rtrn_msg = ""
                deliv_sense_rtrn_msg = ""
                deliv_nav_rtrn_msg = ""
                seq_num = None

                # update sequence number 
                if SEQ_FIELD in json_obj:
                    seq_num = srv.handle_seq(json_obj)
                    #scan_sense_rtrn_msg[SEQ_FIELD] = seq_num
                    #scan_nav_rtrn_msg[SEQ_FIELD] = seq_num
                    #deliv_sense_rtrn_msg[SEQ_FIELD] = seq_num
                    #deliv_nav_rtrn_msg[SEQ_FIELD] = seq_num
                else: 
                    srv.print_msg(FORMAT_ERR, "SEQ field does not exist!")

                # handle scanner rover sensing message calls
                if SCAN_SENSE in json_obj:
                    scan_sense = json_obj[SCAN_SENSE]
                    # update quota status
                    if ZONE in scan_sense:
                    	if RED and GREEN and BLUE in scan_sense:
                        	srv.store(json.loads('{ "SCAN_SENSE.ZONE": ' + str(scan_sense[ZONE]) + ' }'), json_obj, scan_snsg_col)
                    	else:
                        	srv.print_msg(FORMAT_ERR, "RED, GREEN, or BLUE fields do not exist!")
                    else:
                    	srv.print_msg(FORMAT_ERR, "ZONE field does not exist!")
                    raw_doc = scan_snsg_col.find_one(json.loads('{ "ACTION": { "$exists": true } }'))
                    print(raw_doc)
                    del raw_doc['_id']
                    raw_doc[SEQ_FIELD] = seq_num
                    scan_sense_rtrn_msg = json.dumps(raw_doc) + DELIM
                    print(scan_sense_rtrn_msg)
                    srv.send_msg(s, scan_sense_rtrn_msg)

                
                # handle scanner rover navigation message calls 
                elif SCAN_NAV in json_obj:
                	scan_nav = json_obj[SCAN_NAV]
                	# update position
                	if DISTANCE in scan_nav:
                		srv.store(json.loads('{ "SCAN_NAV.DISTANCE": { "$exists": true } }'), json_obj, scan_nav_col)

                # handle delivery rover navigation message calls
                elif DELIV_NAV in json_obj:
                	deliv_nav = json_obj[DELIV_NAV]
                	# update rover status
                	if STATUS and MESSAGE in deliv_nav:
                		srv.store(json.loads('{ "DELIV_NAV.STATUS": { "$exists": true } }'), json_obj, deliv_nav_col)
                	# update position
                	elif X and Y in deliv_nav:
                		srv.store(json.loads('{ "DELIV_NAV.X": { "$exists": true } }'), json_obj, deliv_nav_col)
                	# update spped and direction
                	elif RIGHT_DIR and LEFT_DIR and RIGHT_SPEED and LEFT_SPEED in deliv_nav:
                		srv.store(json.loads('{ "DELIV_NAV.RIGHT_DIR": { "$exists": true } }'), json_obj, deliv_nav_col)

                # handle delivery rover sensing message calls
                elif DELIV_SENSE in json_obj:
                	deliv_sense = json_obj[DELIV_SENSE]
                	# update rover status
                	if ZONE and STATUS in deliv_sense:
                		srv.store(json.loads('{ "DELIV_SENSE.ZONE": { "$exists": true } }'), json_obj, deliv_snsg_col)

                else:
                	srv.print_msg(FORMAT_ERR, "Message is not generated from a specific rover task!") 

                break

        except ValueError as err:
            print(err)
            break
