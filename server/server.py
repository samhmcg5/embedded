#!/usr/bin/env python3

"""
A simple echo server that handles some exceptions
"""
from pymongo import MongoClient
import srv_msg_def as srv
import sys
import socket
import json

# Server configurations
IP_ADDR = '192.168.1.123'
PORT = 2000

# Message data fields
DELIM, SEQ_FIELD, RECV, SENT, FORMAT_ERR = srv.get_msg_constants()
SCAN_SENSE = srv.get_rover_msg_fields()
RED, BLUE, GREEN = srv.get_scanner_rover_fields()

# Database collecitions

# Initialize and connect to database 
mongo = None
if srv.is_db_online() is not True:	
    mongo = srv.connect_to_mongo()

# Main server processing
while True:

    # Initialize and start server listening for clients
    buf = ""
    s = None
    client = None
    address = None
    msg_length = None
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
                rtrn_msg = ""
                seq_num = None
                print(json_obj)
                sys.stdout.flush()

                # update sequence number 
                if SEQ_FIELD in json_obj:
                    seq_num = srv.handle_seq(json_obj)
                else: 
                    srv.print_msg(FORMAT_ERR, "SEQ field does not exist!")

                # handle scanner rover sensing message calls
                if SCAN_SENSE in json_obj:
                     colors = json_obj[SCAN_SENSE]
                     if RED in colors:
                        srv.store("{ SCAN_SENSE:  }", json_obj, mongo)
                break

        except ValueError as err:
            print(err)
            break
