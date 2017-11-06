import PyQt5.QtWidgets as qt
import PyQt5.QtCore as qtcore
import sys

class DeliveryTestGui(qt.QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        taskbox = qt.QVBoxLayout();
        databox = qt.QVBoxLayout()

        colorSel = qt.QComboBox()
        colorSel.addItem("Red")
        colorSel.addItem("Green")
        colorSel.addItem("Blue")
        zoneSel = qt.QComboBox()
        zoneSel.addItem("One")
        zoneSel.addItem("Two")
        zoneSel.addItem("Three")
        okButton = qt.QPushButton("Send Task")
        taskbox.addWidget(colorSel)
        taskbox.addWidget(zoneSel)
        taskbox.addSpacing(46)
        taskbox.addWidget(okButton)
        taskbox.addStretch(1)

        magLabel = qt.QLabel("Magnet State")
        magSel = qt.QComboBox()
        magSel.addItem("On")
        magSel.addItem("Off")
        irLabel = qt.QLabel("IR Distance")
        sendButton = qt.QPushButton("Send Data")
        irSel = qt.QSpinBox()
        databox.addWidget(magLabel)
        databox.addWidget(magSel)
        databox.addWidget(irLabel)
        databox.addWidget(irSel)
        databox.addWidget(sendButton)
        databox.addStretch(1)

        hbox = qt.QHBoxLayout()
        hbox.addLayout(taskbox)
        hbox.addLayout(databox)

        textBox = qt.QTextEdit()
        vbox = qt.QVBoxLayout()
        vbox.addStretch(1)
        vbox.addLayout(hbox)
        vbox.addWidget(textBox)
        
        self.setLayout(vbox)   

        self.setGeometry(300, 300, 600, 400)
        self.setWindowTitle('Delivery Testing')    
        self.show()




if __name__ == '__main__':
    app = qt.QApplication(sys.argv)
    ex = DeliveryTestGui()
    sys.exit(app.exec_())
