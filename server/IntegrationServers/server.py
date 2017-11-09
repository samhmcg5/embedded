# Server class
from database import Database
import socket

# Simple server with default backlog and size of 1
# Connects to a local mongodb service 
class Server():
    def __init__(self, parent, addr, port):
        # Initialize server configuration settings
        self.parent         = parent
        self.addr           = addr
        self.port           = port
        self.size           = 1
        self.backlog        = 1
        # Initialize database configuration settings
        self.db             = Database(self.parent)
        # Initialize client configuration settings
        self.client         = None
        self.client_addr    = None
        # Initialize socket configuration settings
        self.socket         = None
        self.srvonline      = False
        # Initialize message constants

    def sendToStatus(self, msg):
        self.parent.sendToStatus(msg)

    # DATABASE METHODS
    #--------------------------------------------------------------------
    # Begin connection to database
    def db_init(self):
        while self.db.isDBOnline() is False:
            try:
                self.db.connect()
            except ConnectionError as err:
                self.sendToStatus(str(err))
        self.db.db_init()

    # Close and reset database
    def db_close(self):
        if self.db.isDBOnline():
            self.db.disconnect()
        else:
            raise ConnectionError("Database is already offline")

    # Store json_obj in database, returns true if successful
    def store(self, criteria, json_obj, colName):
        return self.db.store(criteria, json_obj, colName)

    # Retrieves json_obj from database, returns json_obj
    def retrieve(self, criteria, colName):
        return self.db.retrieve(criteria, colName)
    #--------------------------------------------------------------------

    # SERVER METHODS
    #--------------------------------------------------------------------
    # Initializing socket options
    def socket_init(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind socket to port and start listening for client connection
    def start(self):
        self.socket.bind((self.addr, self.port))
        self.socket.listen(self.backlog)
        self.sendToStatus("Server is listening on %s:%s" % (self.addr, self.port))

    # Is server online
    def isSRVOnline(self):
        return self.srvonline

    # Connect to client
    def connect(self):
        self.sendToStatus("Waiting for client connection ...")
        if self.isSRVOnline():
            raise ConnectionError("Server is already online")
        else:
            (self.client, self.client_addr) = self.socket.accept()
            self.srvonline = True   
        self.sendToStatus("Client connected on %s:%s" % (self.addr, self.port))
    
    # Disconnect from client
    def disconnect(self):
        if self.isSRVOnline():
            self.sendToStatus("Attempting to disconnect ...")
            self.socket.close()
            self.srvonline = False
            self.sendToStatus("Disconnected ...")
        else:
            raise ConnectionError("Server is offline")

    # Reset server to inital settings
    def reset(self):
        self.socket      = None
        self.client      = None
        self.client_addr = None
        self.srvonline   = False

    # Send message to client, returns true if successfully sent
    def sendmsg(self, msg):
        if self.isSRVOnline():
            try:
                ret = self.client.send( msg.encode() )
                self.sendToStatus("Sent Message: %s" % msg)
            except ConnectionError:
                self.srvonline = False
                raise ConnectionError("Socket is disconnected!")
        else:
            raise ConnectionError("Server is offline")
        return True

    # Receive message from client
    def recvmsg(self):
        data = None
        if self.isSRVOnline():
            try:
                data = self.client.recv(self.size).decode()
            except ConnectionError:
                self.srvonline = False
                raise ConnectionError("socket is disconnected!")
        else:
            raise ConnectionError("Server is offline")
        return data 

    #--------------------------------------------------------------------