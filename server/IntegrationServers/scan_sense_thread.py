from base_thread import ServerBaseThread
from database_fields import ScanSenseFields as SSF
from database_fields import ScanNavFields as SNF
from PyQt5.QtCore import pyqtSignal
import time

class ScanSenseThread(ServerBaseThread):
    # QT Signal - first arg is an int indicating zone # (0,1, or 2)
    #           - second arg is a dict with the numbers .. {"R":0, "G":0, "B":0}
    # emit this signal when you finish scanning a zone ?
    zoneNumbersSignal = pyqtSignal(int, dict)
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "ScanSense"
        
    def handleZONE(self, scansense):
        criteria = ""
        if scansense[SSF.tok_zone] == 0:
            criteria = SSF.crit_zone_a
        elif scansense[SSF.tok_zone] == 1:
            criteria = SSF.crit_zone_b
        elif scansense[SSF.tok_zone] == 2:
            criteria = SSF.crit_zone_c
        else:
            self.sendToStatus("ERROR: Bad zone data")
            return
        json_obj  = {criteria : scansense}
        self.srv.store({criteria : {"$exists":True}}, json_obj, SSF.col_name)
        self.zoneNumbersSignal.emit(scansense[SSF.tok_zone], scansense)


    def handleJSON(self, json_obj):
        if not SSF.token in json_obj:
           return

        if self.recv_seq+1 != json_obj["SEQ"]:
            self.sendToStatus("ERROR: Unexpected sequence number")
        self.recv_seq = json_obj["SEQ"]

        scansense = json_obj[SSF.token]
        # now take an action based on the data ...
        
        if SSF.tok_zone and SSF.tok_type in scansense:
            if SSF.tok_type == "FINISHED ZONE":
                self.handleZONE(scansense)
            else:
                action = 2 # continue scanning
                xpos = self.srv.retrieve({SNF.crit_dist, {"$exists":True}}, self.srv.db.scan_nav)
                zone = 0
                if xpos:
                    if xpos < 40:
                        zone = 1
                    elif xpos >= 40 and xpos < 80:
                        zone = 2
                    elif xpos >= 80:
                        zone = 3 
                if zone != 0 and action == 2:
                    self.srv.sendmsg(ScanSenseMsgs.updated_pos % (self.seq_num, zone, action, xpos))
                    self.seq_num += 1

class ScanSenseMsgs:
    updated_pos = "{\"seq\":%i, \"ZONE\":%i, \"ACTION\":%i, \"XPOS\":%i}"  
