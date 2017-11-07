from base_thread import ServerBaseThread
from server_fields import *

class DelivSenseThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivSense"

    def handleJSON(self, json_obj):
        if not DELIV_SENSE in json_obj:
           return
        delivsense = json_obj[DELIV_SENSE]
        # now take an action based on the data ...

