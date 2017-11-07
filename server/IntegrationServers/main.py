from deliv_nav_thread   import DelivNavThread
from deliv_sense_thread import DelivSenseThread
from scan_nav_thread    import ScanNavThread
from scan_sense_thread  import ScanSenseThread
from status_thread      import StatusThread
from system_gui         import SystemGui

from PyQt5.QtWidgets import QApplication
import sys

if __name__ == '__main__':
    status = StatusThread()

    dnt = DelivNavThread(status)
    dst = DelivSenseThread(status)
    snt = ScanNavThread(status)
    sst = ScanSenseThread(status)

    status.start()
    dnt.start()
    dst.start()
    snt.start()
    sst.start()

    app = QApplication(sys.argv)
    ex  = SystemGui()
    sys.exit(app.exec_())

