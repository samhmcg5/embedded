from base_thread import ServerBaseThread
from server_fields import *

class DelivNavThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivNav"

    def handleJSON(self, json_obj):
        if not DELIV_NAV in json_obj:
           return
        delivnav = json_obj[DELIV_NAV]
        # now take an action based on the data ...
        self.sendToStatus(str(delivnav))
