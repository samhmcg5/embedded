import srv_msg_def as srv
from threading import Thread
from collections import deque
import json
import time

# for passing global data
status_d = deque()

#################################
###### SERVER THREAD BASE #######
#################################
class ServerThreadBase(Thread):
    # constructor
    def __init__(self, port, ip_addr):
        self.port    = port
        self.ip_addr = ip_addr
        # connect to DB
        self.mongo = None
        # if srv.is_db_online() is not True:  
        #     self.mongo = srv.connect_to_mongo()
        #     self.scan_snsg_col, self.scan_nav_col, self.deliv_snsg_col, self.deliv_nav_col  = srv.get_collections()
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        self.msg_length = None
        self.seq_num    = 1
        # call the base class init
        Thread.__init__(self)

    # read from the socket until data is not available, BLOCKING
    def readFromClient(self):
        buf = ""
        # read data until we see the '!' delimeter
        while srv.DELIM not in buf:
            self.msg_length = len(buf)
            data = srv.recv_msg(self.client, self.msg_length)
            if not data:
                raise ConnectionError('Client not connected! Reconnecting...')
            buf += data

            # if *HELLO* seen scrap the data
            if srv.init_msg(buf) is True:
                buf = ""
                break
        msg = srv.get_msg(srv.RECV, buf)
        self.sendToStatusThread(msg)
        # remove delimeter to create JSON object
        buf = buf[:-1] 
        return buf

    # start the connection to the client
    def initClient(self):
        self.seq_num = 1
        if srv.is_srv_online() is not True:  
            self.s = srv.start_server(self.ip_addr, self.port)
            self.client, self.address = srv.client_connect(self.s)

    # OVERRIDE ME !!!!!
    def handleJSON(self, json_obj):
        #since this is the "virtual" one, it should do nothing...
        return

    # Main thread method
    def run(self):
        self.sendToStatusThread("on port %s"%self.port)
        # Main server processing
        while True:
            self.sendToStatusThread("awaiting connection...")
            # Initialize and start server listening for clients
            # run this on dropped connections
            self.initClient()
            self.sendToStatusThread("connected!")
            # Handle messages
            while True:
                try:
                    # blocking read call
                    buf = self.readFromClient()
                    json_obj = None
                    # Attempt to parse the JSON
                    if srv.is_json(buf):
                        json_obj = json.loads(buf)
                        # handle the valid JSON object
                        self.handleJSON(json_obj)
                    else:
                        self.sendToStatusThread(srv.ERROR_MSG_FORMAT)
                        #break
                except (ValueError, ConnectionError, KeyboardInterrupt) as err:
                    self.s.close()
                    srv.clean_db()
                    self.sendToStatusThread("Caught Exception " + err)
                    break


#################################
####### DELIVER NAVIGATE ########
#################################
# instantiate, then call start to run
class DelivNavThread(ServerThreadBase):
    # class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)

    # send the message to the outgoing queue
    def sendToStatusThread(self, msg):
        status_d.append("[DelivNavThread]: " + msg)

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        # do whatever with the incoming data...
        return


#################################
####### DELIVERY SENSIN #########
#################################
class DelivSenseThread(ServerThreadBase):
    # class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)

    # send the message to the outgoing queue
    def sendToStatusThread(self, msg):
        status_d.append("[DelivSenseThread]: " + msg)

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        # do whatever with the incoming data...
        return


#################################
####### SCANNER NAVIGATE ########
#################################
class ScanNavThread(ServerThreadBase):
# class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)

    # send the message to the outgoing queue
    def sendToStatusThread(self, msg):
        status_d.append("[ScanNavThread]: " + msg)

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        # do whatever with the incoming data...
        return


#################################
####### SCANNER SENSING #########
#################################
class ScanSenseThread(ServerThreadBase):
# class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)

    # send the message to the outgoing queue
    def sendToStatusThread(self, msg):
        status_d.append("[ScanSenseThread]: " + msg)

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        # do whatever with the incoming data...
        return


#################################
######## STATUS CONSOLE #########
#################################
class StatusConsoleThread(Thread):
    # class constructor
    def __init__(self, port, ip_addr):
        self.port    = port
        self.ip_addr = ip_addr
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        # call the base class init
        Thread.__init__(self)

    # override from base class
    def run(self):
        print("[StatusConsoleThread]: on port %s"%self.port)
        while True:
            try:
                print(status_d.popleft())
            except IndexError:
                continue
        print("[StatusConsoleThread]: exiting...")
        sys.stdout.flush()

