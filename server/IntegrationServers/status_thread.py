from PyQt5.QtCore import QThread

class StatusThread(QThread):
    def __init__(self):
        QThread.__init__(self)

    def __del__(self):
        self.wait()

    def receiveMsg(self,name,msg):
        print("[%s] %s" % (name, msg))

    def run(self):
        print("Status Thread")