from base_thread import ServerBaseThread
from database_fields import ScanSenseFields as SSF
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
        
    def handleJSON(self, json_obj):
        if not SSF.token in json_obj:
           return
        scansense = json_obj['SCAN_SENSE']
        # now take an action based on the data ...
<<<<<<< HEAD
        self.sendToStatus(str(scansense))
||||||| merged common ancestors
        # self.sendToStatus(str(scansense))
=======
        # self.sendToStatus(str(scansense))

        
>>>>>>> 806986304084990966314b43163c0655025df1ad
