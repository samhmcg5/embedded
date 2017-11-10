from base_thread import ServerBaseThread
from database_fields import DelivNavFields as DNF
from PyQt5.QtCore import pyqtSignal

"""

MONGO USAGE:

Each field is stored under a specific search criteria:
--> position  = {"X":0, "Y":0, "OR":0} is stored under "POS"
        which gives us a document {"POS" : {"X":0, "Y":0, "OR":0} }
        use a search dictionary {"POS":{"$exists":True}} to retrieve the last one
--> status = {"STATUS":1}, (0=IDLE,1=EXEC_TASK,2=EXEC_ACTION)
--> set magnet = {"SET_MAG":1}, (1=ON, 0=OFF)

NOTE:

All outgoing data is taken care of in the handleX() functions...
-- since we use blocking receive calls, the only time we send data 
   is immediately after the PIC requests it from the server

"""

class DelivNavThread(ServerBaseThread):
    positionSig   = pyqtSignal(dict)
    taskStatusSig = pyqtSignal(str)
    roverStateSig = pyqtSignal(int)
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivNav"

    def handlePOS(self, delivnav):
        # simply store the position
        json_store = {DNF.crit_pos : delivnav}
        self.srv.store({DNF.crit_pos : {"$exists":True}}, json_store, DNF.col_name)
        # emit a position signal to the GUI thread
        self.positionSig.emit(delivnav)
        # data = self.srv.retrieve({"POS":{"$exists":True}}, self.srv.db.deliv_nav)
        # self.sendToStatus(str(data))

    def handleSTATUS(self, delivnav):
        # store the current status
        json_store = {DNF.crit_status : delivnav[DNF.tok_status]}
        self.srv.store({DNF.crit_status : {"$exists":True}}, json_store, DNF.col_name)
        if delivnav[DNF.tok_status] == 0:
            # emit idle status to the GUI
            self.taskStatusSig.emit("Rover is IDLE")
            # request new task ...
            # send new task to rover ...
            # start timer for task acknowledge status ...
            # TEMPORARY
            self.srv.sendmsg("{\"seq\":%i, \"COLOR\":0, \"ZONE\":0}" % self.seq_num)
            self.seq_num += 1
        elif delivnav[DNF.tok_status] == 1: # TASK
            # add logic to get last sent task ...
            self.taskStatusSig.emit("Executing TASK: ")
        elif delivnav[DNF.tok_status] == 2: # ACTION
            # add logic to get last sent action ...
            self.taskStatusSig.emit("Executing ACTION: ")

    def handleMAG(self, delivnav):
        json_store = {DNF.crit_mag : delivnav[DNF.tok_mag]}
        self.srv.store({DNF.crit_mag : {"$exists":True}}, json_store, DNF.col_name)
        # now request the state of the magnet ...
        # send the state of the magnet back to the rover ...
        # REMOVE ME
        # self.srv.sendmsg("{\"seq\":%i, \"MAGNET\":%i, \"IR\":1}" % (self.seq_num,delivnav["SET_MAGNET"]) )
        # self.seq_num += 1

    def handleSTATE(self, delivnav):
        # store the state
        json_store = {DNF.crit_state : delivnav[DNF.tok_state]}
        self.srv.store({DNF.crit_state : {"$exists":True}}, json_store, DNF.col_name)
        # tell the GUI what my state is
        self.roverStateSig.emit(delivnav[DNF.tok_state])

    # QT SLOT
    def transmitPosUpdate(self, x, y, ori):
        msg = "{\"seq\":%i, \"CORR_X\":%i, \"CORR_Y\":%i, \"CORR_O\":%i}" % (self.seq_num, x, y, ori)
        self.seq_num += 1
        self.srv.sendmsg(msg)

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
        # HANDLE THE STATE OF THE ROVER
        elif DNF.tok_state in delivnav.keys():
            self.handleSTATE(delivnav)
