from deliv_nav_thread   import DelivNavThread
from deliv_sense_thread import DelivSenseThread
from scan_nav_thread    import ScanNavThread
from scan_sense_thread  import ScanSenseThread
from status_thread      import StatusThread
from system_gui         import SystemGui

from PyQt5.QtWidgets import QApplication
import sys

# IP_ADDR = 'localhost'
IP_ADDR = '192.168.1.123'
print("INITIALIZING TEAM14 SERVER @ %s" % IP_ADDR)

if __name__ == '__main__':
    status = StatusThread()

    dnt = DelivNavThread(IP_ADDR, 2000, status)
    dst = DelivSenseThread(IP_ADDR, 2001, status)
    snt = ScanNavThread(IP_ADDR, 2002, status)
    sst = ScanSenseThread(IP_ADDR, 2003, status)

    status.start()
    dnt.start()
    dst.start()
    snt.start()
    sst.start()

    app = QApplication(sys.argv)
    ex  = SystemGui(status, dnt, dst, snt, sst)
    sys.exit(app.exec_())

