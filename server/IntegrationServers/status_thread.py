from PyQt5.QtCore import QThread

class StatusThread(QThread):
    def __init__(self):
        QThread.__init__(self)

    def __del__(self):
        self.wait()

    def receiveMsg(self,name,msg):
        fmt = '[{0:10}] {1:}'.format(name,msg)
        if "ScanSense" in fmt or "DIST" in fmt:
            print(fmt)

    def run(self):
        return 