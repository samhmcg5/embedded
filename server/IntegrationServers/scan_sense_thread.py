from base_thread import ServerBaseThread
from server_fields import *

class ScanSenseThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "ScanSense"

    def handleJSON(self, json_obj):
        if not SCAN_SENSE in json_obj:
           return
        scansense = json_obj[SCAN_SENSE]
        # now take an action based on the data ...
        # self.sendToStatus(str(scansense))
