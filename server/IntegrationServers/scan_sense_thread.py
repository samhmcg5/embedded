from base_thread import ServerBaseThread
from database_fields import ScanSenseFields as SSF
from PyQt5.QtCore import pyqtSignal
import time

class ScanSenseThread(ServerBaseThread):
    # QT Signal - first arg is an int indicating zone # (0,1, or 2)
    #           - second arg is a dict with the numbers .. {"R":0, "G":0, "B":0}
    # emit this signal when you finish scanning a zone ?
    zoneNumbersSignal = pyqtSignal(int, dict)
    def __init__(self, ip, port, status_thread, vrb):
        ServerBaseThread.__init__(self, ip, port, status_thread, vrb)
        self.name = "ScanSense"
        
    def sendStartMsg(self):
        try:
            msg = "START MESSAGE..."
            self.srv.sendmsg(msg)
            self.seq_num += 1
        except ConnectionError as err:
            self.sendToStatus("ERROR: %s" % str(err), 5)

    def handleZONE(self, scansense):
        criteria = ""
        if scansense[SSF.tok_zone] == 0:
            criteria = SSF.crit_zone_a
        elif scansense[SSF.tok_zone] == 1:
            criteria = SSF.crit_zone_b
        elif scansense[SSF.tok_zone] == 2:
            criteria = SSF.crit_zone_c
        else:
            self.sendToStatus("ERROR: Bad zone data", 5)
            return
        json_obj  = {criteria : scansense}
        self.srv.store({criteria : {"$exists":True}}, json_obj, SSF.col_name)
        self.zoneNumbersSignal.emit(scansense[SSF.tok_zone], scansense)

    def handleJSON(self, json_obj):
        if not SSF.token in json_obj:
           return

        if self.recv_seq+1 != json_obj["SEQ"]:
            self.sendToStatus("ERROR: Unexpected sequence number", 3)
        self.recv_seq = json_obj["SEQ"]

        scansense = json_obj[SSF.token]
        # now take an action based on the data ...
        
        if SSF.tok_zone in scansense:
            self.handleZONE(scansense)

        