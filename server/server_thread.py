import srv_msg_def as srv
from threading import Thread
from collections import deque
import time

# for passing global data
d = deque()

#################################
####### DELIVER NAVIGATE ########
#################################
# instantiate, then call start to run
class DelivNavThread(Thread):
    # class constructor
    def __init__(self, port, ip_addr):
        self.port    = port
        self.ip_addr = ip_addr
        # connect to DB
        self.mongo = None
        # if srv.is_db_online() is not True:  
        #     self.mongo = srv.connect_to_mongo()
        #     self.scan_snsg_col, self.scan_nav_col, self.deliv_snsg_col, self.deliv_nav_col  = srv.get_collections()
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        self.msg_length = None
        self.seq_num    = 1
        # call the base class init
        Thread.__init__(self)

    # override from base class
    def run(self):
        print("DelivNavThread on port %s"%self.port)
        d.append("Thread sleeps for 2 seconds")
        time.sleep(2)
        #print("Thread %d woke up"%(secs))
        d.append("Thread 2 woke up")


#################################
####### DELIVERY SENSIN #########
#################################

class DelivSenseThread(Thread):
    # class constructor
    def __init__(self, port, ip_addr):
        self.port    = port
        self.ip_addr = ip_addr
        # connect to DB
        # self.mongo = None
        # if srv.is_db_online() is not True:  
        #     self.mongo = srv.connect_to_mongo()
        #     self.scan_snsg_col, self.scan_nav_col, self.deliv_snsg_col, self.deliv_nav_col  = srv.get_collections()
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        self.msg_length = None
        self.seq_num    = 1
        # call the base class init
        Thread.__init__(self)

    # override from base class
    def run(self):
        print("DelivSenseThread on port %s"%self.port)
        d.append("Thread sleeps for 3 seconds")
        time.sleep(3)
        #print("Thread %d woke up"%(secs))
        d.append("Thread 3 woke up")




#################################
####### SCANNER NAVIGATE ########
#################################

class ScanNavThread(Thread):
    # class constructor
    def __init__(self, port, ip_addr):
        self.port    = port
        self.ip_addr = ip_addr
        # connect to DB
        self.mongo = None
        # if srv.is_db_online() is not True:  
        #     self.mongo = srv.connect_to_mongo()
        #     self.scan_snsg_col, self.scan_nav_col, self.deliv_snsg_col, self.deliv_nav_col  = srv.get_collections()
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        self.msg_length = None
        self.seq_num    = 1
        # call the base class init
        Thread.__init__(self)

    # override from base class
    def run(self):
        print("ScanNavThread on port %s"%self.port)
        d.append("Thread sleeps for 4 seconds")
        time.sleep(4)
        d.append("Thread 4 woke up")


#################################
####### SCANNER SENSING #########
#################################

class ScanSenseThread(Thread):
    # class constructor
    def __init__(self, port, ip_addr):
        self.port    = port
        self.ip_addr = ip_addr
        # connect to DB
        self.mongo = None
        # if srv.is_db_online() is not True:  
        #     self.mongo = srv.connect_to_mongo()
        #     self.scan_snsg_col, self.scan_nav_col, self.deliv_snsg_col, self.deliv_nav_col  = srv.get_collections()
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        self.msg_length = None
        self.seq_num    = 1
        # call the base class init
        Thread.__init__(self)

    # override from base class
    def run(self):
        print("ScanSenseThread on port %s"%self.port)
        d.append("Thread sleeps for 5 seconds")
        time.sleep(5)
        d.append("Thread 5 woke up")


#################################
######## STATUS CONSOLE #########
#################################

class StatusConsoleThread(Thread):
    # class constructor
    def __init__(self, port, ip_addr):
        self.port    = port
        self.ip_addr = ip_addr
        # initialize some members
        self.client     = None
        self.address    = None
        self.s          = None
        # call the base class init
        Thread.__init__(self)

    # override from base class
    def run(self):
        print("StatusConsoleThread on port %s"%self.port)
        while True:
            try:
                print(d.popleft())
            except IndexError:
                continue
        print("StatusConsoleThread exiting...")

