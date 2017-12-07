from PyQt5.QtCore import QThread

class StatusThread(QThread):
    def __init__(self):
        QThread.__init__(self)

    def __del__(self):
        self.wait()

    def receiveMsg(self,name,msg):
        fmt = '[{0:10}] {1:}'.format(name,msg)
        if "IR" in fmt or "TEST" in fmt:
            if not "DelivNav" in fmt:
                print(fmt)

    def run(self):
        return 