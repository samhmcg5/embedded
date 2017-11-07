from base_thread import ServerBaseThread

class ScanNavThread(ServerBaseThread):
    def __init__(self, ip, port, status_thread):
        ServerBaseThread.__init__(self, ip, port, status_thread)
        self.name = "ScanNav"

    