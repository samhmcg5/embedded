from PyQt5.QtWidgets import QWidget, QApplication
import PyQt5.QtWidgets as qtw
import PyQt5.QtCore as qtc
from PyQt5.QtGui import QFont
import sys

from server import *
from database_fields import GuiFields as GF
from styles import stylesheet

###############################
### TOP LEVEL GUI COMPONENT ###
###############################
FONT_SIZE = 18
class SystemGui(QWidget):
    statusSig = qtc.pyqtSignal(str,str)
    def __init__(self, status, dnt, dst, snt, sst):
        super().__init__()
        self.name = "GUI"
        self.threads = {"Status"    : status,
                        "DelivNav"  : dnt,
                        "DelivSense": dst,
                        "ScanNav"   : snt,
                        "ScanSense" : sst
                       }
        self.initUI()
        self.setStyleSheet(stylesheet)
        self.connectSignals()
        self.sendToStatus("Gui Initialized ...")
        self.show()
        self.initDB()

    def connectSignals(self):
        # OUTGOING signal to status thread
        self.statusSig.connect(self.threads["Status"].receiveMsg)
        # INCOMING signals from DELIV_NAV thread
        self.threads["DelivNav"].positionSig.connect(self.delivstats.posGrid.setCurrentPos)
        self.threads["DelivNav"].taskStatusSig.connect(self.delivstats.execStatus.setStatus)
        self.threads["DelivNav"].roverStateSig.connect(self.handleDelivRoverState)
        # OUTGOING signals to DELIV_NAV thread
        self.delivstats.posCorrectSig.connect(self.threads["DelivNav"].transmitPosUpdate)
        # connect button CLICKED to DB STORE
        self.quotaframe.sendButton.clicked.connect(self.storeQuotaInDB)
        # INCOMING signal from SCAN_SENSE
        self.threads["ScanSense"].zoneNumbersSignal.connect(self.currentnums.updateZone)
        self.threads["DelivNav"].zoneNumbersSignal.connect(self.currentnums.updateZone)
        # OUTGOING to scan threads
        #self.start.clicked.connect(self.threads['ScanSense'].sendStartMsg)
        self.start.clicked.connect(self.threads['ScanNav'].sendStartMsg)
    
    def initDB(self):
        # we just need the server to expose the database calls
        # I'm gonna pretend that the server is actually a DB...
        self.db  = Server(self, '', 0)
        self.db.db_init()
        # init zones to 0
        zoneA = {GF.crit_zone_a : {"RED":0, "GREEN":0, "BLUE":0}}
        zoneB = {GF.crit_zone_b : {"RED":0, "GREEN":0, "BLUE":0}}
        zoneC = {GF.crit_zone_c : {"RED":0, "GREEN":0, "BLUE":0}}
        self.db.store({GF.crit_zone_a : {"$exists":True}}, zoneA, GF.col_name)
        self.db.store({GF.crit_zone_b : {"$exists":True}}, zoneB, GF.col_name)
        self.db.store({GF.crit_zone_c : {"$exists":True}}, zoneC, GF.col_name)

    def sendToStatus(self, msg):
            self.statusSig.emit(self.name, msg)

    def storeQuotaInDB(self):
        # gather the data from the spin boxes
        zoneA = {GF.crit_zone_a : self.quotaframe.zoneA.getValues()}
        zoneB = {GF.crit_zone_b : self.quotaframe.zoneB.getValues()}
        zoneC = {GF.crit_zone_c : self.quotaframe.zoneC.getValues()}
        # store the data
        self.db.store({GF.crit_zone_a : {"$exists":True}}, zoneA, GF.col_name)
        self.db.store({GF.crit_zone_b : {"$exists":True}}, zoneB, GF.col_name)
        self.db.store({GF.crit_zone_c : {"$exists":True}}, zoneC, GF.col_name)
        # self.threads['DelivNav'].generateNextTask()


    def handleDelivRoverState(self, state):
        if state in [1,3,4,6]:
            self.delivstats.setPosCorrectionLocked(True)
        else:
            self.delivstats.setPosCorrectionLocked(False)
    
    def initUI(self): 
        f = QFont()
        f.setPointSize(FONT_SIZE)
        self.setFont(f)
        self.start       = qtw.QPushButton("START")
        self.quotaframe  = SetQuotaFrame()
        self.currentnums = CurrentNumbersFrame()
        self.delivstats  = DelivNavStatusFrame()
        vbox = qtw.QVBoxLayout()
        vbox.addWidget(self.start, alignment=qtc.Qt.AlignRight)
        vbox.addWidget(self.currentnums)
        vbox.addWidget(self.quotaframe)
        vbox.addWidget(self.delivstats)
        self.setLayout(vbox)
        self.setGeometry(600, 100, 800, 500)
        self.setWindowTitle('Server Control')

######################
### SET THE QUOTAS ###
######################
class SetQuotaFrame(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
    def initUI(self):
        vbox = qtw.QVBoxLayout()
        self.zoneA = QuotaRow("Zone A")
        self.zoneB = QuotaRow("Zone B")
        self.zoneC = QuotaRow("Zone C")
        self.sendButton = qtw.QPushButton("Send New Quotas")
        vbox.addWidget(qtw.QLabel("SET QUOTAS"))
        vbox.addWidget(self.zoneA)
        vbox.addWidget(self.zoneB)
        vbox.addWidget(self.zoneC)
        vbox.addWidget(self.sendButton, alignment=qtc.Qt.AlignRight)
        self.setLayout(vbox)

class QuotaRow(QWidget):
    def __init__(self, name):
        super().__init__()
        self.name = name
        self.initUI()
    def initUI(self):
        self.red = SpinBoxQuota("Red",5)
        self.green = SpinBoxQuota("Green",5)
        self.blue = SpinBoxQuota("Blue",5)
        hbox = qtw.QHBoxLayout()
        hbox.addWidget(qtw.QLabel(self.name + ":\t"))
        hbox.addWidget(self.red)
        hbox.addWidget(self.green)
        hbox.addWidget(self.blue)
        self.setLayout(hbox)
    def getValues(self):
        ret_dict = {"RED"  : self.red.value(),
                    "GREEN": self.green.value(),
                    "BLUE" : self.blue.value()}
        return ret_dict

class SpinBoxQuota(qtw.QSpinBox):
    def __init__(self, pre, maxim):
        super().__init__()
        self.setRange(0,maxim)
        self.setPrefix(pre + ": ")

###############################
### DISPLAY CURRENT NUMBERS ###
###############################
class CurrentNumbersFrame(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
    def initUI(self):
        self.zoneA = NumsRow("Zone A")
        self.zoneB = NumsRow("Zone B")
        self.zoneC = NumsRow("Zone C")
        vbox = qtw.QVBoxLayout()
        vbox.addWidget(qtw.QLabel("CURRENT NUMBERS"))
        vbox.addWidget(self.zoneA)
        vbox.addWidget(self.zoneB)
        vbox.addWidget(self.zoneC)
        self.setLayout(vbox)
    def updateZone(self, zone, vals):
        if zone == 0:
            self.zoneA.setNums(vals["RED"],vals["GREEN"],vals["BLUE"])
        if zone == 1:
            self.zoneB.setNums(vals["RED"],vals["GREEN"],vals["BLUE"])
        if zone == 2:
            self.zoneC.setNums(vals["RED"],vals["GREEN"],vals["BLUE"])

class NumsRow(QWidget):
    def __init__(self, name):
        super().__init__()
        self.name = name
        self.initUI()
    def initUI(self):
        self.red = qtw.QLabel("Red = ...")
        self.green = qtw.QLabel("Green = ...")
        self.blue = qtw.QLabel("Blue = ...")
        hbox = qtw.QHBoxLayout()
        hbox.addWidget(qtw.QLabel(self.name + ":\t"))
        hbox.addWidget(self.red)
        hbox.addWidget(self.green)
        hbox.addWidget(self.blue)
        self.setLayout(hbox)
    def setNums(self, r, g, b):
        self.red.setText("Red = %i" % r)
        self.green.setText("Green = %i" % g)
        self.blue.setText("Blue = %i" % b)

#############################
### DELIVERY NAVIG STATUS ###
#############################
class DelivNavStatusFrame(QWidget):
    posCorrectSig = qtc.pyqtSignal(int, int, int)
    def __init__(self):
        super().__init__()
        self.initUI()
        self.connectSignals()

    def connectSignals(self):
        self.posGrid.sendButton.clicked.connect(self.handleSendPos)

    def handleSendPos(self):
        # gather data
        x   = self.posGrid.nx.value()
        y   = self.posGrid.ny.value()
        ori = self.posGrid.nori.value()
        # emit new signal
        self.posCorrectSig.emit(x,y,ori)

    def setPosCorrectionLocked(self,lock):
        self.posGrid.locked = lock
        self.posGrid.nx.setReadOnly(lock)
        self.posGrid.ny.setReadOnly(lock)
        self.posGrid.nori.setReadOnly(lock)
        self.posGrid.sendButton.setEnabled(~lock)
        # this is a fucken hack, fix this in the stylesheet
        self.posGrid.sendButton.setText("" if lock else "Send")

    def initUI(self):
        vbox = qtw.QVBoxLayout()
        self.execStatus = ExecStatus()
        self.posGrid = PositionGrid()
        vbox.addWidget(qtw.QLabel("DELIVERY STATUS"))
        vbox.addWidget(self.execStatus)
        vbox.addWidget(self.posGrid)
        self.setLayout(vbox)

class ExecStatus(QWidget):
    def __init__(self):
        super().__init__()
        self.status = qtw.QLabel("...waiting for connection...")
        hbox = qtw.QHBoxLayout()
        hbox.addWidget(qtw.QLabel("Currently: "))
        hbox.addWidget(self.status, alignment=qtc.Qt.AlignLeft, stretch=1)
        self.setLayout(hbox)
    def setStatus(self, msg):
        self.status.setText(msg)

class PositionGrid(QWidget):
    def __init__(self): 
        super().__init__()
        self.locked = False
        self.initUI()

    def setCurrentPos(self, pos):
        self.x.setText("X: %i" % pos["X"])
        self.y.setText("Y: %i" % pos["Y"])
        self.ori.setText("OR: %i" % pos["OR"])
        if self.locked:
            self.nx.setValue(pos["X"])
            self.ny.setValue(pos["Y"])
            self.nori.setValue(pos["OR"])

    def initUI(self):
        self.x    = qtw.QLabel("X: ...")
        self.y    = qtw.QLabel("Y: ...")
        self.ori  = qtw.QLabel("OR: ...")
        self.nx   = SpinBoxQuota("X",90)
        self.ny   = SpinBoxQuota("Y",50)
        self.nori = SpinBoxQuota("OR",359)
        self.sendButton = qtw.QPushButton("Send")
        gbox = qtw.QGridLayout()
        gbox.addWidget(qtw.QLabel("Current:"), 0,0)
        gbox.addWidget(self.x,0,1)
        gbox.addWidget(self.y,0,2)
        gbox.addWidget(self.ori,0,3)
        gbox.addWidget(qtw.QLabel("Corrected:"), 1,0)
        gbox.addWidget(self.nx,1,1)
        gbox.addWidget(self.ny,1,2)
        gbox.addWidget(self.nori,1,3)
        gbox.addWidget(self.sendButton,1,4)
        self.setLayout(gbox)



