from PyQt5.QtCore import QThread, pyqtSignal
import json

from server import *
from database_fields import *
import server_defs as defs
import re

"""

This class is the base for each of the threads connecting to the PICs. 
It handles the connections, the signals/slots, server send/secv, and DB calls.

Override the handleJSON() function in the inherited classes. This function is called by 
the run() function once valid data is received over TCP/IP. At this point, you can take action on the available
data such as storing it, doing some calculation, or sending data back to the pic

"""

class ServerBaseThread(QThread):
    statusSig = pyqtSignal(str,str)
    def __init__(self, ip, port, status_thread):
        QThread.__init__(self)
        self.name = "BaseThread"
        self.srv  = Server(self, ip, port)
        self.status_thread = status_thread
        self.seq_num  = 1
        self.recv_seq = -1
        self.connectSignals()

    def __del__(self):
        self.wait()

    def connectSignals(self):
        self.statusSig.connect(self.status_thread.receiveMsg)

    def sendToStatus(self, msg):
        self.statusSig.emit(self.name, msg)

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

    # read from the socket until data is not available, BLOCKING
    def readFromClient(self):
        buf = ""
        # read data until we see the '!' delimeter
        while '!' not in buf:
            buf += self.srv.recvmsg()
            # if *HELLO* seen scrap the data
            if "*HELLO*" in buf:
                buf = re.sub("\*HELLO\*","" , buf)
        buf = buf[:-1]
        # self.sendToStatus(str("Received: " + buf))
        return buf

    def run(self):
        self.sendToStatus("Thread Starting ...")
        # initialize the database connection
        self.srv.db_init()
        # Main processing loop :
        while True:
            self.initClient()
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
                        self.sendToStatus("ERROR: Received bad JSON")
                except ConnectionError as err:
                        self.srv.reset()
                        self.srv.db.clean()
                        break
