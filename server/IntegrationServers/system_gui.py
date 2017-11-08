from PyQt5.QtWidgets import *
from PyQt5.QtCore import pyqtSignal
import sys

class SystemGui(QWidget):
    statusSig = pyqtSignal(str,str)
    def __init__(self, status, dnt, dst, snt, sst):
        super().__init__()
        self.threads = {"Status"    : status,
                        "DelivNav"  : dnt,
                        "DelivSense": dst,
                        "ScanNav"   : snt,
                        "ScanSense" : sst
                       }
        self.name = "GUI"
        self.initUI()
        self.connectSignals()
        self.sendToStatus("Gui Initialized ...")

    def connectSignals(self):
        self.statusSig.connect(self.threads["Status"].receiveMsg)

    def sendToStatus(self, msg):
        self.statusSig.emit(self.name, msg)

    def initUI(self):
        self.setGeometry(300, 300, 300, 300)
        self.setWindowTitle('Delivery System')
        self.show()