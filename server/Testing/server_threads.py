from threading import Thread
from collections import deque
import defines_status_viewer as status
import re
from server import *
from database_fields import *
from server_fields import *
import server_defs as defs
# for passing global data
status_d = deque()

#################################
###### SERVER THREAD BASE #######
#################################
class ServerThreadBase(Thread):
    # constructor
    def __init__(self, port, ip_addr):
        # Instantiate server instance
        self.srv = Server(ip_addr, port)
        self.seq_num    = 1     # sequence to send
        self.recv_seq   = 0     # last sequence received
        # call the Thread class init
        Thread.__init__(self)

    # send the message to the outgoing queue
    def sendToStatusThread(self, msg):
        status_d.append(msg)

    # read from the socket until data is not available, BLOCKING
    def readFromClient(self):
        buf = ""
        # read data until we see the '!' delimeter
        while delim not in buf:
            try:
                buf += self.srv.recvmsg()
            except ConnectionError as err:
                raise ConnectionError(err)
            # if *HELLO* seen scrap the data
            if buf is WIFLY_INIT_MSG:
                buf = ""
                break

        buf = buf[:-1]
        msg = status.StatusMsg(self.name, "RECVD", "NONE", "Received msg: %s" % buf)
        self.sendToStatusThread(msg)
        return buf

    # start the connection to the client
    def initClient(self):
        self.seq_num = 1
        self.srv.socket_init()
        self.srv.start()
        self.srv.connect()

    # OVERRIDE ME !!!!!
    def handleJSON(self, json_obj):
        #since this is the "virtual" one, it should do nothing...
        return

    # Main thread method
    def run(self):
        self.srv.db_init()
        text = re.sub('\"', '', "on port %s" % self.srv.port)
        msg = status.StatusMsg(self.name, "SERVER", "NONE", text)
        self.sendToStatusThread(msg)
        # Main server processing
        while True:
            msg = status.StatusMsg(self.name, "SERVER", "NONE", "awaiting connection...")
            self.sendToStatusThread(msg)
            # Initialize and start server listening for clients
            # run this on dropped connections
            self.initClient()
            msg = status.StatusMsg(self.name, "SERVER", "NONE", "connected!")
            self.sendToStatusThread(msg)
            # Handle messages
            while True:
                try:
                    # blocking read call
                    buf = self.readFromClient()
                    json_obj = None
                    # Attempt to parse the JSON
                    if defs.isjson(buf):
                        json_obj = json.loads(buf)
                        # handle the valid JSON object
                        self.handleJSON(json_obj)
                    else:
                        msg = status.StatusMsg(self.name, "ERROR", "BAD_JSON", "Message is not json object type")
                        self.sendToStatusThread(msg)

                except (ValueError, ConnectionError, KeyboardInterrupt) as err:
                    self.srv.reset()
                    self.srv.db.clean_db()
                    text = re.sub('\"', '', "Caught Exception " + str(err))
                    msg = status.StatusMsg(self.name, "ERROR", "EXCEPTION", text)
                    self.sendToStatusThread(msg)
                    break


#################################
####### DELIVER NAVIGATE ########
#################################
# instantiate, then call start to run
class DelivNavThread(ServerThreadBase):
    # class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        # do whatever with the incoming data...
        if not DELIV_NAV in json_obj:
           return
        deliv_nav = json_obj[DELIV_NAV]
        # update rover status
        if STATUS and MESSAGE in deliv_nav:
            text = "Data being stored: " + json.dumps(json_obj)
            text = re.sub('\"','',text)
            msg = status.StatusMsg(self.srv.db.deliv_nav, "STORE", "COLOR", text)
            self.sendToStatusThread(msg)
            text = self.srv.store(self.srv.db.deliv_nav_col, json.loads('{ "DELIV_NAV.STATUS": { "$exists": true } }'), json_obj, self.srv.db.deliv_nav)
            text = re.sub('\"','',text)
            msg = status.StatusMsg(self.name, "SUCCESS", "STATUS", text)
            self.sendToStatusThread(msg)

        # update position
        elif srv.X and srv.Y in deliv_nav:
            text = srv.INFO_DB_STORE_ATT + " Data being stored: " + json.dumps(json_obj)
            text = re.sub('\"','',text)
            msg = status.StatusMsg(self.name, "STORE", "COLOR", text)
            self.sendToStatusThread(msg)
            text = self.srv.store(json.loads('{ "DELIV_NAV.X": { "$exists": true } }'), json_obj)
            text = re.sub('\"','',text)
            msg = status.StatusMsg(self.name, "SUCCESS", "LOC", text)
            self.sendToStatusThread(msg)

        # update spped and direction
        elif srv.RIGHT_DIR and srv.LEFT_DIR and srv.RIGHT_SPEED and srv.LEFT_SPEED in deliv_nav:
            text = srv.INFO_DB_STORE_ATT + " Data being stored: " + json.dumps(json_obj)
            text = re.sub('\"','',text)
            msg = status.StatusMsg(self.name, "STORE", "COLOR", text)
            self.sendToStatusThread(msg)
            text = srv.store(json.loads('{ "DELIV_NAV.RIGHT_DIR": { "$exists": true } }'), json_obj, self.col)
            text = re.sub('\"','',text)
            msg = status.StatusMsg(self.name, "SUCCESS", "SPEED", text)
            self.sendToStatusThread(msg)

        if srv.STATUS in deliv_nav:
            if deliv_nav[srv.STATUS] is 0:
                # send message for next action (FWD, BACKWARD, etc)
                #deliv_nav_rtrn_msg = srv.retrieve(self.seq_num, self.col)
                srv.send_msg(self.client, srv.DELIV_NAV_DEFAULT_ACTION)
                self.seq_num += 1
        return


#################################
####### DELIVERY SENSIN #########
#################################
class DelivSenseThread(ServerThreadBase):
    # class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)
        self.name = "DeliverySense"
        #self.col  = self.mongo_client[srv.DATABASE_NAME][srv.DELIVERY_SENSING_COL_NAME]

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        # do whatever with the incoming data...
        if 'IRDIST' in json_obj[srv.DELIV_SENSE] and json_obj[srv.DELIV_SENSE]['IRDIST'] >= 500:
            if self.recv_seq % 2 is 0:
                srv.send_msg(self.client, '{"SEQ": '+str(self.seq_num)+', "ACTION": 1}!')
            else:
                srv.send_msg(self.client, '{"SEQ": '+str(self.seq_num)+', "ACTION": 0}!')
            self.seq_num += 1
        return


#################################
####### SCANNER NAVIGATE ########
#################################
class ScanNavThread(ServerThreadBase):
# class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)
        self.name = "ScannerNav"
        #self.col  = self.mongo_client[srv.DATABASE_NAME][srv.SCANNER_NAVIGATION_COL_NAME]

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        if srv.SCAN_NAV in json_obj:
            if self.seq_num % 20 is 0:
                srv.send_msg(self.client, '{ "SEQ": ' + str(self.seq_num) +', "ACTION": 1, "DIST": 3, "SPEED":25 }')
            self.seq_num += 1
        return


#################################
####### SCANNER SENSING #########
#################################
class ScanSenseThread(ServerThreadBase):
# class constructor
    def __init__(self, port, ip_addr):
        ServerThreadBase.__init__(self, port, ip_addr)
        self.name = "ScannerSense"
        self.col  = self.mongo[srv.DATABASE_NAME][srv.SCANNER_SENSING_COL_NAME]

    # overridden from base, put Mongo Logic here
    def handleJSON(self, json_obj):
        # handle scanner rover sensing message calls
        if srv.SCAN_SENSE in json_obj:
            scan_sense = json_obj[srv.SCAN_SENSE]
            # update quota status
            if srv.ZONE in scan_sense:
                if scan_sense[srv.ZONE] is 0:
                    self.client.send(('{ "SEQ": ' + str(self.seq_num) + ', "ZONE": 1, "ACTION": 1 }').encode())
                
                if srv.RED and srv.GREEN and srv.BLUE in scan_sense:
                    text = srv.INFO_DB_STORE_ATT + " Data being stored: " + json.dumps(json_obj)
                    text = re.sub('\"','',text)
                    msg = status.StatusMsg(self.name, "STORE", "COLOR", text)
                    self.sendToStatusThread(msg)

                    text = srv.store(json.loads('{ "SCAN_SENSE.ZONE": ' + str(scan_sense[srv.ZONE]) + ' }'), json_obj, self.col)
                    # text = re.sub('\"','',text)
                    msg = status.StatusMsg(self.name, "SUCCESS", "COLOR", text)
                    self.sendToStatusThread(msg)

                if json_obj[srv.SEQ_FIELD] <= 100 and json_obj[srv.SEQ_FIELD] >= 0:
                    self.client.send(str('{ "SEQ": ' + str(self.seq_num) + ', "ZONE": ' +  str(1) + ' , "ACTION": 1 }').encode())

                elif json_obj[srv.SEQ_FIELD] <= 200 and json_obj[srv.SEQ_FIELD] > 100:
                    self.client.send(str('{ "SEQ": ' + str(self.seq_num) + ', "ZONE": ' +  str(2) + ' , "ACTION": 1 }').encode())

                elif json_obj[srv.SEQ_FIELD] <= 300 and json_obj[srv.SEQ_FIELD] > 200:
                    self.client.send(str('{ "SEQ": ' + str(self.seq_num) + ', "ZONE": ' +  str(3) + ' , "ACTION": 1 }').encode())

                elif json_obj[srv.SEQ_FIELD] >= 301:
                    self.client.send(str('{ "SEQ": ' + str(self.seq_num) + ', "ZONE": 1 , "ACTION": 1 }').encode())
                    


#################################
######## STATUS CONSOLE #########
#################################
class StatusConsoleThread(Thread):
    # class constructor
    def __init__(self, port, ip_addr, verbose=False):
        self.port    = port
        self.ip_addr = ip_addr
        self.verbose = verbose
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        # call the base class init
        Thread.__init__(self)

    # start the connection to the client
    def initClient(self):
        self.seq_num = 1
        if srv.is_srv_online() is not True:  
            self.s = srv.start_server(self.ip_addr, self.port)
            self.client, self.address = srv.client_connect(self.s)

    # override from base class
    def run(self):
        if self.verbose:
            print("[StatusConsoleThread]: on port %s"%self.port)
        else:
            print("[StatusConsoleThread]: Running in verbose mode")

        while True:
            if not seld.verbose:
                self.initClient()
            while True:
                try:
                    recvd = status_d.popleft()
                    if self.verbose:
                        print(status.msgToString(recvd))
                    else:
                        json_msg = status.STATUS_JSON%(recvd.origin, recvd.mtype, recvd.subj, recvd.text)
                        srv.send_msg(self.client, json_msg)
                except IndexError:
                    continue
            print("[StatusConsoleThread]: exiting...")
            sys.stdout.flush()

