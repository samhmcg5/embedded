from PyQt5.QtWidgets import *
import sys

class SystemGui(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setGeometry(300, 300, 300, 300)
        self.setWindowTitle('Delivery System')
        self.show()