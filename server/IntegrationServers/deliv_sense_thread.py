from base_thread import ServerBaseThread
from database_fields import DelivSenseFields as DSF
from database_fields import DelivNavFields as DNF


class DelivSenseThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivSense"

    def handleMAG(self, delivsense):
        json_mag = {DSF.crit_mag : delivsense[DSF.tok_mag]}
        json_ir = {DSF.crit_ir : delivsense[DSF.tok_ir]}
        self.srv.store({DSF.crit_mag : {"$exists":True}}, json_mag, DSF.col_name)
        self.srv.store({DSF.crit_ir : {"$exists":True}}, json_ir, DSF.col_name)
 
        mag = self.srv.retrieve({DNF.crit_mag : {"$exists":True}}, self.srv.db.deliv_nav)
        if mag:
            print ("--> ", mag, json_mag)
            self.srv.sendmsg('{"SEQ": 0, "ACTION": %i}!' % mag['SET_MAG'])
 

    # def handleIR(self, delivsense):
        # json_obj = {DSF.crit_obj : delivsense[DSF.tok_obj]}
        # self.srv.store({DSF.crit_obj : {"$exists":True}}, json_obj, DSF.col_name)

    def handleJSON(self, json_obj):
        if not DSF.token in json_obj:
           return #error
        if self.recv_seq+1 != json_obj["SEQ"]:
            self.sendToStatus("ERROR: Unexpected sequence number")
        self.recv_seq = json_obj["SEQ"]
        
        delivsense = json_obj[DSF.token]
        # now take an action based on the data ...
        
        # if DSF.tok_ir in delivsense.keys():
        #     self.handleIR(delivsense)
        #receiving current magnet state, and ir distance. Need to send message in format of 
        if DSF.tok_mag in delivsense.keys():
            self.handleMAG(delivsense)
    

