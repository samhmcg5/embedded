from base_thread import ServerBaseThread
from database_fields import DelivSenseFields as DSF


class DelivSenseThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "DelivSense"

    def handleMAG(self, delivsense):
        json_mag = {DSF.crit_mag : delivsense[tok_mag]}
        self.srv.store({DSF.crit_mag : {"$exists":True}}, json_mag, DSF.col_name)

    def handleIR(self, delivsense):
        json_ir = {DSF.crit_ir : delivsense[tok_ir]}
        json_obj = {DSF.crit_obj : delivsense[tok_obj]}
        self.srv.store({DSF.crit_ir : {"$exists":True}}, json_ir, DSF.col_name)
        self.srv.store({DSF.crit_obj : {"$exists":True}}, json_obj, DSF.col_name)

    def handleJSON(self, json_obj):
        if not DSF.token in json_obj:
           return #error
        if self.recv_seq+1 != json_obj["SEQ"]:
            self.sendToStatus("ERROR: Unexpected sequence number")
        self.recv_seq = json_obj["SEQ"]
        
        delivsense = json_obj[DELIV_SENSE]
        # now take an action based on the data ...

        if DSF.tok_mag in delivsense.keys():
            self.handleMAG(delivsense)
        elif DSF.tok_ir in delivsense.keys():
            self.handleIT(delivsense)

