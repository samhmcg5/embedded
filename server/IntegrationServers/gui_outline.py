from PyQt5.QtWidgets import QWidget, QApplication
import PyQt5.QtWidgets as qtw
import PyQt5.QtCore as qtc
from PyQt5.QtGui import QFont
import sys

from styles import stylesheet

###############################
### TOP LEVEL GUI COMPONENT ###
###############################
FONT_SIZE = 18
class TopLevelUI(QWidget):
    statusSig = qtc.pyqtSignal(str,str)
    def __init__(self):
        super().__init__()
        self.initUI()
        self.setStyleSheet(stylesheet)
    def initUI(self): 
        f = QFont()
        f.setPointSize(FONT_SIZE)
        self.setFont(f)
        self.quotaframe  = SetQuotaFrame()
        self.currentnums = CurrentNumbersFrame()
        self.delivstats  = DelivNavStatusFrame()
        vbox = qtw.QVBoxLayout()
        vbox.addWidget(self.quotaframe)
        vbox.addWidget(self.currentnums)
        vbox.addWidget(self.delivstats)
        self.setLayout(vbox)
        self.setGeometry(300, 300, 800, 500)
        self.setWindowTitle('Server Control')
        self.show()

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

class NumsRow(QWidget):
    def __init__(self, name):
        super().__init__()
        self.name = name
        self.initUI()
    def initUI(self):
        self.red = qtw.QLabel("Red = 0")
        self.green = qtw.QLabel("Green = 0")
        self.blue = qtw.QLabel("Blue = 0")
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
    def __init__(self):
        super().__init__()
        self.initUI()
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
        self.status = qtw.QLabel("IDLE")
        hbox = qtw.QHBoxLayout()
        hbox.addWidget(qtw.QLabel("Currently: "))
        hbox.addWidget(self.status, alignment=qtc.Qt.AlignLeft, stretch=1)
        self.setLayout(hbox)
    def setStatus(msg):
        self.status.setText(msg)

class PositionGrid(QWidget):
    def __init__(self): 
        super().__init__()
        self.initUI()
    def initUI(self):
        self.x    = qtw.QLabel("X: 0")
        self.y    = qtw.QLabel("Y: 20")
        self.ori  = qtw.QLabel("OR: 0")
        self.nx   = SpinBoxQuota("X",90)
        self.ny   = SpinBoxQuota("Y",50)
        self.nori = SpinBoxQuota("OR",359)
        self.sendButton = qtw.QPushButton("Send Position")
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


########################
### EXECUTE THE MAIN ###
########################
if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex  = TopLevelUI()
    sys.exit(app.exec_())
