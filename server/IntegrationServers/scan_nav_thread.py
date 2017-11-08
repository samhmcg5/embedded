from base_thread import ServerBaseThread
from database_fields import ScanNavFields as SNF

class ScanNavThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "ScanNav"

    def handleJSON(self, json_obj):
        if not SNF.token in json_obj:
           return
        scannav = json_obj[SCAN_NAV]
        # now take an action based on the data ...
