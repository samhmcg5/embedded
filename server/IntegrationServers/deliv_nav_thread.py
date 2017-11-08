from base_thread import ServerBaseThread
from server_fields import *


# Try this:
# collection.update({'_id':book["_id"]},book)  

class DelivNavThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivNav"

    def handleJSON(self, json_obj):
        if not DELIV_NAV in json_obj:
           return
        
        if self.recv_seq+1 != json_obj["SEQ"]:
            self.sendToStatus("Unexpected sequence number")
        self.recv_seq = json_obj["SEQ"]
        # get the data out of it
        delivnav = json_obj[DELIV_NAV]

        # now take an action based on the data ...
        # POSTITION
        # if "X" in delivnav.keys():
            # self.srv.store({"X":{"$exists":True}}, delivnav, self.srv.db.deliv_nav)
            # data = self.srv.retrieve({"X":{"$exists":True}}, self.srv.db.deliv_nav)
            # self.sendToStatus(str(data))
        # IDLE, NEED A TASK
        if "STATUS" in delivnav.keys():
            dictstr = {"STATUS":{"$exists":"true"}}
            json_obj = {"STATUS":json_obj}
            self.srv.store(dictstr, json_obj, self.srv.db.deliv_nav)
            data = self.srv.retrieve(dictstr, self.srv.db.deliv_nav)
            self.sendToStatus(str(data))
            # if "IDLE" in delivnav["MSG"]:
                # Get a task
                # ensure the task is eventually acked, maybe use a time out period?
        # SET MAGNET ON OR OFF
        # elif "SET_MAGNET" in delivnav.keys():            
            # self.srv.store({"SET_MAGNET":{"$exists":True}}, delivnav, self.srv.db.deliv_nav)

