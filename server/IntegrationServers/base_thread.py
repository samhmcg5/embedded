from PyQt5.QtCore import QThread, pyqtSignal

class ServerBaseThread(QThread):
    statusSig = pyqtSignal(str,str)
    def __init__(self, status_thread):
        QThread.__init__(self)
        self.name = "BaseThread"
        self.status_thread = status_thread
        self.connectSignals()

    def __del__(self):
        self.wait()

    def connectSignals(self):
        self.statusSig.connect(self.status_thread.receiveMsg)

    def sendToStatus(self, msg):
        self.statusSig.emit(self.name, msg)

    def run(self):
        sendToStatus("Base Running")
