#!/usr/bin/env python3

"""
A simple echo server that handles some exceptions
"""
from pymongo import MongoClient
import bsonjs
import math
import pprint as pprint
import srv_msg_def as srv
import sys
import socket
import json
import time

# Server configurations
IP_ADDR = '192.168.1.122'
PORT = 2000

# Message data fields
DELIM, SEQ_FIELD, RECV, SENT, FORMAT_ERR = srv.get_msg_constants()
SCAN_NAV, DISTANCE = srv.get_scanner_navigation_fields()
SCAN_SENSE, ZONE, RED, BLUE, GREEN = srv.get_scanner_sensing_fields()
DELIV_NAV, STATUS, MESSAGE, X, Y, RIGHT_DIR, LEFT_DIR, RIGHT_SPEED, LEFT_SPEED = srv.get_delivery_navigation_fields()
DELIV_SENSE = srv.get_delivery_sensing_fields()

# Database collecitions

# Initialize and connect to database 
global mongo
global scan_snsg_col
mongo = None
if srv.is_db_online() is not True:	
    mongo = srv.connect_to_mongo()
scan_snsg_col, scan_nav_col, deliv_snsg_col, deliv_nav_col  = srv.get_collections()

global s
global client
global address
global msg_length
global seq_num

s = None
client = None
address = None
msg_length = None
    
# Main server processing
while True:
    # Initialize and start server listening for clients
    seq_num = 0
    buf = ""
    if srv.is_srv_online() is not True:  
    	s = srv.start_server(IP_ADDR, PORT)
    	client, address = srv.client_connect(s)
    
    # Handle messages
    while True:
        try:
            # Receive data byte by byte and add to buffer
            msg_length = len(buf)
            #t0 = time.clock()
            data = srv.recv_msg(client, msg_length)
            if not data:
            	raise ConnectionError('SERVER ERROR: Client is not connected! Attempting to reconnect...')
            buf += data
            while True:
                if srv.init_msg(buf) is True:
                    buf = ""
                    break
                if DELIM not in buf: # dont begin handling until delimeter recognized
                    break

                #t1 = time.clock() - t0;
                #print(t1, " seconds")
                #sys.stdout.flush()
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

                # update sequence number 
                seq_num = srv.handle_seq(seq_num)
                
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
                    
                    # send message back of current action (STOP or CONTINUE)
                    #scan_sense_rtrn_msg = srv.retrieve(seq_num, scan_snsg_col)
                    #temp_send = input("Send? ")
                    #zone = input("Zone: ")
                    #if temp_send is 'y':
                    	#print("Message sent: ", '{ "SEQ": ' + str(seq_num) + ', "ZONE": ' +  zone + ' , "ACTION": 1 }!')
                    	#sys.stdout.flush()

                    print(json_obj[SEQ_FIELD])
                    sys.stdout.flush()
                    if json_obj[SEQ_FIELD] <= 100 and json_obj[SEQ_FIELD] >= 0:
                        print('{ "SEQ": ' + str(seq_num) + ', "ZONE": ' +  str(1) + ' , "ACTION": 1 }')
                        sys.stdout.flush()
                        client.send(str('{ "SEQ": ' + str(seq_num) + ', "ZONE": ' +  str(1) + ' , "ACTION": 1 }').encode())

                    elif json_obj[SEQ_FIELD] <= 200 and json_obj[SEQ_FIELD] > 100:
                        print('{ "SEQ": ' + str(seq_num) + ', "ZONE": ' +  str(2) + ' , "ACTION": 1 }')
                        sys.stdout.flush()
                        client.send(str('{ "SEQ": ' + str(seq_num) + ', "ZONE": ' +  str(2) + ' , "ACTION": 1 }').encode())

                    elif json_obj[SEQ_FIELD] <= 300 and json_obj[SEQ_FIELD] > 200:
                        print('{ "SEQ": ' + str(seq_num) + ', "ZONE": ' +  str(3) + ' , "ACTION": 1 }')
                        sys.stdout.flush()
                        client.send(str('{ "SEQ": ' + str(seq_num) + ', "ZONE": ' +  str(3) + ' , "ACTION": 1 }').encode())

                    elif json_obj[SEQ_FIELD] >= 301:
                        print('{"SEQ": ' + str(seq_num) + ', "ZONE": 1, "ACTION": 0}')
                        sys.stdout.flush()
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
                	if STATUS in deliv_nav:
                		if deliv_nav[STATUS] is 0:
                			# send message for next action (FWD, BACKWARD, etc)
                			deliv_nav_rtrn_msg = srv.retrieve(seq_num, deliv_nav_col)
                			srv.send_msg(client, deliv_nav_rtrn_msg)

                # handle delivery rover sensing message calls
                elif DELIV_SENSE in json_obj:
                	deliv_sense = json_obj[DELIV_SENSE]
                	# update rover status
                	if ZONE and STATUS in deliv_sense:
                		srv.store(json.loads('{ "DELIV_SENSE.ZONE": { "$exists": true } }'), json_obj, deliv_snsg_col)

                else:
                	srv.print_msg(FORMAT_ERR, "Message is not generated from a specific rover task!") 

                break

        except (ValueError, ConnectionError, KeyboardInterrupt) as err:
        	s.close()
        	srv.clean_db()
        	print(err)
        	break
