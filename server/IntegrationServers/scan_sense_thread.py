from base_thread import ServerBaseThread

class ScanSenseThread(ServerBaseThread):
    def __init__(self, status_thread):
        ServerBaseThread.__init__(self, status_thread)
        self.name = "ScanSense"

    def run(self):
        self.sendToStatus("ScanSense")
