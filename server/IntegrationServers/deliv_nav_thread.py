from base_thread import ServerBaseThread
from database_fields import DelivNavFields as DNF
from database_fields import DelivSenseFields as DSF
from database_fields import ScanSenseFields as SSF
from database_fields import GuiFields as GUI
from PyQt5.QtCore import pyqtSignal
import time
from collections import deque

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

COLORS = ["RED", "GREEN", "BLUE"]
ZONES  = ["ONE", "TWO", "THREE"]
LOCS   = [38, 78, 110]

class DelivNavThread(ServerBaseThread):
    positionSig   = pyqtSignal(dict)
    taskStatusSig = pyqtSignal(str)
    roverStateSig = pyqtSignal(int)
    zoneNumbersSignal = pyqtSignal(int, dict)
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivNav"
        self.prev_task = "..."
        self.prev_zone = 2 # initialize to 2 so we start with zone 0
        self.send_time = 0
        self.prev_set = None
        self.stack = deque()

    # INCOMING message handler
    def handlePOS(self, delivnav):
        json_store = {DNF.crit_pos : delivnav}
        self.srv.store({DNF.crit_pos : {"$exists":True}}, json_store, DNF.col_name)
        self.positionSig.emit(delivnav)

    # INCOMING message handler
    def handleSTATUS(self, delivnav):
        # store the current status
        json_store = {DNF.crit_status : delivnav[DNF.tok_status]}
        self.srv.store({DNF.crit_status : {"$exists":True}}, json_store, DNF.col_name)
        if delivnav[DNF.tok_status] == 0:
            self.taskStatusSig.emit("Rover is IDLE")
            try:
                # task = self.generateNextTask()
                task = self.stack.popleft()
                self.prev_set = task
                if task is not None:
                    # calculate where to put the block
                    dist = LOCS[task[0]] - 9*self.getNumInZone(task[0])
                    # print("-->", dist)
                    # im still gonna Send it 
                    self.srv.sendmsg(DelivNavMsgs.task % (self.seq_num, task[1], dist))
                    self.seq_num += 1
                    self.prev_zone = task[0]
                    self.prev_task = DelivNavMsgs.prev_task_str % (COLORS[task[1]],ZONES[task[0]])
            except RuntimeError as err:
                self.sendToStatus("ERROR: %s" % str(err))
            except IndexError:
                self.sendToStatus("ERROR: Unable to retrieve a task")
        elif delivnav[DNF.tok_status] == 1: # TASK
            self.taskStatusSig.emit("Executing TASK: %s" % self.prev_task)
        elif delivnav[DNF.tok_status] == 2: # ACTION
            self.taskStatusSig.emit("Executing ACTION: %s" % self.prev_task)

    # INCOMING message handler
    def handleMAG(self, delivnav):
        json_store = {DNF.crit_mag : delivnav[DNF.tok_mag]}
        self.srv.store({DNF.crit_mag : {"$exists":True}}, json_store, DNF.col_name)
        # now request the state of the magnet ...
        mag = self.srv.retrieve({DSF.crit_mag : {"$exists":True}}, self.srv.db.deliv_sense)
        ir  = self.srv.retrieve({DSF.crit_ir : {"$exists":True}}, self.srv.db.deliv_sense)
        if mag and ir:
            self.srv.sendmsg(DelivNavMsgs.data % (self.seq_num,mag['MAGNET'],ir['IRDIST']))
            self.seq_num += 1

    # INCOMING message handler
    def handleSTATE(self, delivnav):
        # store the state
        json_store = {DNF.crit_state : delivnav[DNF.tok_state]}
        self.srv.store({DNF.crit_state : {"$exists":True}}, json_store, DNF.col_name)
        # tell the GUI what my state is
        self.roverStateSig.emit(delivnav[DNF.tok_state])
        # update the database with new numbers
        if delivnav[DNF.tok_state] in [5,6] and self.prev_set is not None:
            # print ("--> ", self.prev_set)
            # self.updateZoneData(self.prev_set[1], self.prev_set[0])
            self.prev_set = None

    def getNumInZone(self, zone):
        criteria = self.getCriteria(zone)
        if not criteria: return
        data  = self.srv.retrieve({criteria : {"$exists":True}}, self.srv.db.scan_sense)
        if not data: return
        return (data[criteria]['RED'] + data[criteria]['GREEN'] + data[criteria]['BLUE'])

    def getCriteria(self, zone):
        criteria = ""
        if zone == 0:
            criteria = SSF.crit_zone_a
        elif zone == 1:
            criteria = SSF.crit_zone_b
        elif zone == 2:
            criteria = SSF.crit_zone_c
        else:
            return
        return criteria  

    def updateZoneData(self, color, zone):
        criteria = self.getCriteria(zone)
        if not criteria: 
            return
        data  = self.srv.retrieve({criteria : {"$exists":True}}, self.srv.db.scan_sense)
        # print ("--> ", zone, color, data)
        if not data:
            # print ("--> ERROR")
            return
        if color == 0:
            data[criteria]['RED'] += 1
        elif color == 1:
            data[criteria]['GREEN'] += 1
        elif color == 2:
            data[criteria]['BLUE'] += 1
        self.srv.store({criteria : {"$exists":True}}, data, SSF.col_name)
        self.zoneNumbersSignal.emit(zone, data[criteria])
        # print("--> ", data)


    # QT SLOT
    def transmitPosUpdate(self, x, y, ori):
        try:
            msg = DelivNavMsgs.corrected_pos % (self.seq_num, x, y, ori)
            self.srv.sendmsg(msg)
            self.seq_num += 1
        except ConnectionError as err:
            self.sendToStatus("ERROR: %s" % str(err))

    # helper function
    def generateNextTask(self):
        try:
            # Get the data
            diffs = []
            diffs.append( self.getDifference(SSF.crit_zone_a) )
            diffs.append( self.getDifference(SSF.crit_zone_b) )
            diffs.append( self.getDifference(SSF.crit_zone_c) )
            # iterate to find next task, start with a new zone...
            prev = self.prev_zone
            zone = prev+1 if prev<2 else 0
            for i in range(3):
                for color in diffs[zone].keys():
                    # print("-->", zone, color, diffs[zone][color])
                    if diffs[zone][color] < 0:
                        if color == "RED":
                            color = 0
                        elif color == "GREEN":
                            color = 1
                        else:
                            color = 2
                        # return (zone, color)
                        self.stack.append((zone, color))
                zone = zone+1 if zone<2 else 0
        except RuntimeError as err:
            self.sendToStatus("ERROR: %s" % str(err))
            return

    # helper function
    def getDifference(self, crit):
        data  = self.srv.retrieve({crit : {"$exists":True}}, self.srv.db.scan_sense)
        quota = self.srv.retrieve({crit : {"$exists":True}}, self.srv.db.gui)
        if not data or not quota:
            raise RuntimeError("Zone data unavailable")
        data  = data[crit]
        quota = quota[crit]
        diff  = {"RED"  : data[SSF.tok_r] - quota[SSF.tok_r],
                 "GREEN": data[SSF.tok_g] - quota[SSF.tok_g],
                 "BLUE" : data[SSF.tok_b] - quota[SSF.tok_b]}
        return diff

    # OVERRIDDEN from base class
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
        # elif 'TEST' in delivnav.keys():
        #    print("-->", delivnav['TEST'])

class DelivNavMsgs:
    corrected_pos = "{\"seq\":%i, \"CORR_X\":%i, \"CORR_Y\":%i, \"CORR_O\":%i}"
    task          = "{\"seq\":%i, \"COLOR\":%i, \"ZONE\":%i}"
    data          = "{\"seq\":%i, \"MAGNET\":%i, \"IR\":%i}"
    prev_task_str = "%s block to zone %s"
