from pymongo import MongoClient
import pprint as pprint
import srv_msg_def as srv
import sys
import socket
import json

from server_thread import *

IP_ADDR = '192.168.1.123'

# Instantiate all the thread objects ( calls __init__() )
statusThread    = StatusConsoleThread(2004, IP_ADDR)
delivNavThr     = DelivNavThread(2000,IP_ADDR)
delivSenseThr   = DelivSenseThread(2001,IP_ADDR)
scanNavThr      = ScanNavThread(2002,IP_ADDR)
scanSenseThr    = ScanSenseThread(2003,IP_ADDR)

# Start the thread main loop by calling start()
statusThread.start()
delivNavThr.start()  
delivSenseThr.start()
scanNavThr.start()   
scanSenseThr.start() 

