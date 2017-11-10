from base_thread import ServerBaseThread
from database_fields import ScanNavFields as SNF

class ScanNavThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "ScanNav"

    # store the distance and state into Mongo
    def handleDIST(self, scannav):
        json_dist  = {SNF.crit_dist : scannav["DIST"]}
        json_state = {SNF.crit_dist : scannav["STATE"]}
        self.srv.store({SNF.crit_dist : {"$exists":True}}, json_dist, SNF.col_name)
        self.srv.store({SNF.crit_state : {"$exists":True}}, json_state, SNF.col_name)

    def handleJSON(self, json_obj):
        if not SNF.token in json_obj:
           return

        if self.recv_seq+1 != json_obj["SEQ"]:
            self.sendToStatus("ERROR: Unexpected sequence number")
        self.recv_seq = json_obj["SEQ"]

        scannav = json_obj[SCAN_NAV]
        # now take an action based on the data ...

        if SNF.tok_dist in scannav:
            self.handleDIST(scannav)
