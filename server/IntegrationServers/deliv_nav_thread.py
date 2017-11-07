from base_thread import ServerBaseThread

class DelivNavThread(ServerBaseThread):
    def __init__(self, status_thread):
        ServerBaseThread.__init__(self, status_thread)
        self.name = "DelivNav"

    def run(self):
        self.sendToStatus("DelivNav")
