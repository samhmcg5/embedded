import PyQt5.QtWidgets as qt
import PyQt5.QtCore as qtcore
from PyQt5.QtGui import QFont
import sys
import socket
import time
import json
import re

class ClientThread(qtcore.QThread):
    updateSig = qtcore.pyqtSignal(dict)
    def __init__(self, parent=None):
        self.parent = parent
        qtcore.QThread.__init__(self)
        self.connect()
    def __del__(self):
        self.wait()

    def connect(self):
        host = '192.168.1.123'
        port = 2000
        backlog = 5
        self.outgoing = ""
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((host,port))
        s.listen(backlog)
        print("Waiting for a connection:")
        self.client, address = s.accept()
        print("Connected to WiFly:")

    def setOutgoing(self, data):
        print('--> RECVD = ', data)
        self.outgoing = data

    def run(self):
        data = ""
        buffer = ""
        while True:
            while data != "!":
                data = self.client.recv(1).decode()
                buffer += data
            buffer = buffer[:-1]
            if "*HELLO*" in buffer:
                buffer = re.sub("\*HELLO\*","" , buffer)
            json_obj = json.loads(buffer)
            self.updateSig.emit(json_obj)
            # print(json_obj)
            buffer = ""
            data = ""

            if self.outgoing is not "":
                self.client.send(self.outgoing.encode())
                self.outgoing = ""


class DeliveryTestGui(qt.QWidget):
    sendSignal = qtcore.pyqtSignal(str)
    def __init__(self):
        self.seq = 0
        self.client = ClientThread(self)
        self.client.start()
        super().__init__()
        self.initUI()
        self.connectSignals()
        self.x   = 0
        self.y   = 0
        self.ori = 0
        self.deliv_state = 0

    def connectSignals(self):
        self.okButton.clicked.connect(self.sendTask)
        self.sendButton.clicked.connect(self.sendData)
        self.offset_but.clicked.connect(self.sendOffset)
        self.sendSignal.connect(self.client.setOutgoing)
        self.client.updateSig.connect(self.updateData)

    def updateData(self, json):
        if "STATUS" in json["DELIV_NAV"].keys():
            if "IDLE" in json['DELIV_NAV']['MSG']:
                self.stat_label.setText("Status: \t\t%s"%"IDLE")
        elif "TASK" in json["DELIV_NAV"].keys() or "ACTION" in json["DELIV_NAV"].keys():
            self.stat_label.setText("Status: \t%s"%"EXECUTING")
        elif "Y" in json["DELIV_NAV"].keys():
            self.updatePos(json)
        elif "SET_MAGNET" in json["DELIV_NAV"].keys():
            mg = "ON" if json["DELIV_NAV"]["SET_MAGNET"] else "OFF"
            self.mag_label.setText("SetMag: \t\t%s"%mg)
            self.ir_label.setText("IR Data: \t%i"%json["DELIV_NAV"]["IR_DATA"])
        elif "STATE" in json["DELIV_NAV"].keys():
            self.deliv_state = json["DELIV_NAV"]["STATE"]
            if self.deliv_state in [1,3,4,6]:
                self.lockOffsets(self.x,self.y,self.ori)
            else:
                self.unlockOffsets()

    def sendTask(self):
        color = self.colorSel.currentIndex()
        zone  = self.zoneSel.currentIndex()
        print("--> Sending Task: color = %i, zone = %i" % (color,zone))
        msg = "{\"seq\":%i, \"COLOR\":%i, \"ZONE\":%i}" % (self.seq, color, zone)
        self.seq += 1
        self.sendSignal.emit(msg)

    def sendData(self):
        mag = self.magSel.currentIndex()
        ir = self.irSel.value()
        print("--> Sending Data: mag = %i, ir = %i" % (mag,ir))
        msg = "{\"seq\":%i, \"MAGNET\":%i, \"IR\":%i}" % (self.seq, mag, ir)
        self.seq += 1
        self.sendSignal.emit(msg)

    def sendOffset(self):
        newx = self.x_off.value()
        newy = self.y_off.value()
        newo = self.or_off.value()
        print("--> Sending revised position...")
        msg = "{\"seq\":%i, \"CORR_X\":%i, \"CORR_Y\":%i, \"CORR_O\":%i}" % (self.seq, newx, newy, newo)
        self.seq += 1
        self.sendSignal.emit(msg)

    def updatePos(self,json):
        self.x   = json["DELIV_NAV"]["X"]
        self.y   = json["DELIV_NAV"]["Y"]
        self.ori = json["DELIV_NAV"]["OR"]
        self.x_label.setText("X: \t%i"%json["DELIV_NAV"]["X"])
        self.y_label.setText("Y: \t%i"%json["DELIV_NAV"]["Y"])
        self.or_label.setText("OR: \t%i"%json["DELIV_NAV"]["OR"])

    def lockOffsets(self,x,y,ori):
        self.x_off.setValue(x)
        self.x_off.setReadOnly(True)
        self.y_off.setValue(y)
        self.y_off.setReadOnly(True)
        self.or_off.setValue(ori)
        self.or_off.setReadOnly(True)
        self.offset_but.setEnabled(False)

    def unlockOffsets(self):
        self.x_off.setReadOnly(False)
        self.y_off.setReadOnly(False)
        self.or_off.setReadOnly(False)
        self.offset_but.setEnabled(True)

    def initUI(self):
        f = QFont()
        f.setPointSize(24)
        self.setFont(f)
        taskbox = qt.QVBoxLayout();
        databox = qt.QVBoxLayout()

        self.colorSel = qt.QComboBox()
        self.colorSel.addItem("Red")
        self.colorSel.addItem("Green")
        self.colorSel.addItem("Blue")
        self.zoneSel = qt.QComboBox()
        self.zoneSel.addItem("One")
        self.zoneSel.addItem("Two")
        self.zoneSel.addItem("Three")
        self.okButton = qt.QPushButton("Send Task")
        taskbox.addWidget(self.colorSel)
        taskbox.addWidget(self.zoneSel)
        taskbox.addSpacing(110)
        taskbox.addWidget(self.okButton)
        taskbox.addStretch(1)

        magLabel = qt.QLabel("Magnet State")
        self.magSel = qt.QComboBox()
        self.magSel.addItem("Off")
        self.magSel.addItem("On")
        irLabel = qt.QLabel("IR Distance")
        self.irSel = qt.QSpinBox()
        self.sendButton = qt.QPushButton("Send Data")
        databox.addWidget(magLabel)
        databox.addWidget(self.magSel)
        databox.addWidget(irLabel)
        databox.addWidget(self.irSel)
        databox.addWidget(self.sendButton)
        databox.addStretch(1)

        hbox = qt.QHBoxLayout()
        hbox.addLayout(taskbox)
        hbox.addLayout(databox)

        self.x_label    = qt.QLabel("X: ...")
        self.y_label    = qt.QLabel("Y: ...")
        self.or_label   = qt.QLabel("OR: ...")
        self.stat_label = qt.QLabel("Status: ...")
        self.mag_label  = qt.QLabel("SetMag: ...")
        self.ir_label   = qt.QLabel("IR Data: ...")
        gridbox = qt.QGridLayout()
        gridbox.addWidget(self.x_label,0,0)
        gridbox.addWidget(self.y_label,1,0)
        gridbox.addWidget(self.or_label,2,0)
        gridbox.addWidget(self.stat_label,0,1)
        gridbox.addWidget(self.mag_label,1,1)
        gridbox.addWidget(self.ir_label,2,1)

        offset_box = qt.QHBoxLayout()
        offset_box.addWidget(qt.QLabel("X:"))
        self.x_off = qt.QSpinBox()
        self.x_off.setRange(0,90)
        offset_box.addWidget(self.x_off)
        offset_box.addWidget(qt.QLabel("Y:"))
        self.y_off = qt.QSpinBox()
        self.y_off.setValue(20)
        self.y_off.setRange(0,50)
        offset_box.addWidget(self.y_off)
        offset_box.addWidget(qt.QLabel("OR:"))
        self.or_off = qt.QSpinBox()
        self.or_off.setRange(0,359)
        offset_box.addWidget(self.or_off)
        self.offset_but = qt.QPushButton("Send Offset")
        offset_box.addWidget(self.offset_but)

        vbox = qt.QVBoxLayout()
        vbox.addLayout(hbox)
        vbox.addLayout(gridbox)
        vbox.addSpacing(50)
        vbox.addLayout(offset_box)
        self.setLayout(vbox)

        self.setGeometry(300, 300, 800, 500)
        self.setWindowTitle('Delivery Testing')
        self.show()




if __name__ == '__main__':
    app = qt.QApplication(sys.argv)
    ex = DeliveryTestGui()
    sys.exit(app.exec_())
