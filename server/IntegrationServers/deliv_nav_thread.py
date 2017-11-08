from base_thread import ServerBaseThread
from database_fields import DelivNavFields as DNF

"""
MONGO USAGE:

Each field is stored under a specific search criteria:
--> position  = {"X":0, "Y":0, "OR":0} is stored under "POS"
        which gives us a document {"POS" : {"X":0, "Y":0, "OR":0} }
        use a search dictionary {"POS":{"$exists":True}} to retrieve the last one
--> status = {"STATUS":1}, (0=IDLE,1=EXEC_TASK,2=EXEC_ACTION)
--> set magnet = {"SET_MAG":1}, (1=ON, 0=OFF)

"""

class DelivNavThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivNav"

    def handlePOS(self, delivnav):
        # simply store the position
        json_store = {DNF.crit_pos : delivnav}
        self.srv.store({DNF.crit_pos : {"$exists":True}}, json_store, DNF.col_name)
        # data = self.srv.retrieve({"POS":{"$exists":True}}, self.srv.db.deliv_nav)
        # self.sendToStatus(str(data))

    def handleSTATUS(self, delivnav):
        # store the current status
        json_store = {DNF.crit_status : delivnav["STATUS"]}
        self.srv.store({DNF.crit_status : {"$exists":True}}, json_store, DNF.col_name)
        if delivnav["STATUS"] == 0:
            # request new task ...
            # send new task to rover ...
            # start timer for task acknowledge status ...
            pass

    def handleMAG(self, delivnav):
        json_store = {DNF.crit_mag : delivnav["SET_MAGNET"]}
        self.srv.store({DNF.crit_mag : {"$exists":True}}, json_store, DNF.col_name)
        # now request the state of the magnet ...
        # send the state of the magnet back to the rover ...

    # overridden from base class
    def handleJSON(self, json_obj):
        if not DNF.token in json_obj:
           return
        
        if self.recv_seq+1 != json_obj["SEQ"]:
            self.sendToStatus("ERROR: Unexpected sequence number")
        self.recv_seq = json_obj["SEQ"]
        
        # get the data out of it
        delivnav = json_obj[DNF.token]

        # now take an action based on the data ...
        # POSTITION
        if DNF.tok_pos in delivnav.keys():
            self.handlePOS(delivnav)
        # IDLE, NEED A TASK
        elif DNF.tok_status in delivnav.keys():
            self.handleSTATUS(delivnav)
        # SET MAGNET ON OR OFF
        elif DNF.tok_mag in delivnav.keys():            
            self.handleMAG(delivnav)
