from base_thread import ServerBaseThread

class ScanNavThread(ServerBaseThread):
    def __init__(self, status_thread):
        ServerBaseThread.__init__(self, status_thread)
        self.name = "ScanNav"

    def run(self):
        self.sendToStatus("ScanNav")