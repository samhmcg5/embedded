from base_thread import ServerBaseThread

class DelivSenseThread(ServerBaseThread):
    def __init__(self, status_thread):
        ServerBaseThread.__init__(self, status_thread)
        self.name = "DelivSense"

    def run(self):
        self.sendToStatus("DelivSense")
