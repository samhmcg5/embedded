# Database class, constants, and definitions
from pymongo import MongoClient
from pymongo.errors import ServerSelectionTimeoutError
import database_fields as db

class Database():
    def __init__(self, parent):
        self.parent             = parent
        # Database connection and collection handlers
        self.mongoclient = None
        # Database and collection names
        self.dbname             = db.dbname
        self.deliv_nav          = db.deliv_nav
        self.deliv_sense        = db.deliv_sense
        self.scan_nav           = db.scan_nav
        self.scan_sense         = db.scan_sense
        self.gui                = db.gui
        self.clean()

    # Initialize database and collection fields
    def db_init(self):
        # self.sendToStatus(db.INFO_DB_INIT)
        self.database = self.mongoclient[db.dbname]
        self.col_init()

    def sendToStatus(self, msg):
        self.parent.sendToStatus(msg)

    # Initialize collections with default storage fields
    def col_init(self):
        self.deliv_nav_col = self.database[db.deliv_nav]
        self.deliv_nav_col.delete_many({})

        self.deliv_sense_col = self.database[db.deliv_sense]    
        self.deliv_sense_col.delete_many({})
        
        self.scan_nav_col = self.database[db.scan_nav]
        self.scan_nav_col.delete_many({})

        self.scan_sense_col = self.database[db.scan_sense]
        self.scan_sense_col.delete_many({})

        self.gui_col = self.database[db.gui]
        self.gui_col.delete_many({})

    # Connect to database
    def connect(self):
        # self.sendToStatus(db.INFO_DB_CONN_ATT)
        self.mongoclient = MongoClient(db.host, db.port, serverSelectionTimeoutMS=3000)
        try: 
            result = self.mongoclient.admin.command("ismaster")
        except ServerSelectionTimeoutError:
            self.dbonline = False
            raise ConnectionError(db.ERROR_DB_CONN)
        self.dbonline = True
        self.sendToStatus(db.INFO_DB_CONN_SUC%(db.host,db.port) )

    # Disconnect from database
    def disconnect(self):
        # Disconnect mongo
        self.clean()
        self.mongoclient.close()
        self.dbonline = False

    # Empty databse and clear fields
    def clean(self):
        if self.mongoclient is not None:
            self.mongoclient.drop_database(db.dbname)
        self.database        = None
        self.deliv_nav_col   = None
        self.deliv_sense_col = None
        self.scan_nav_col    = None
        self.scan_sense_col  = None
        self.dbonline        = False

    # Check if database is connected
    def isDBOnline(self):
        return self.dbonline

    # Stores a JSON formatted object in the database
    # Return TRUE on successful store
    def store(self, criteria, json_obj, colName):
        self.sendToStatus(db.INFO_DB_STORE_ATT % colName)
        # Replace database field with given criteria with new json_obj data
        col = self.database[colName]
        res = col.replace_one(criteria, json_obj)
        # Criteria does not match database field in a given collection
        if res.matched_count == 0:
            res = col.insert_one(json_obj)
            if res.acknowledged == True:
                self.sendToStatus("New entry %s in %s" % (str(json_obj),colName))
            else:
                raise RuntimeError(db.ERROR_DB_STORE % colName)
        else:
            self.sendToStatus(db.INFO_DB_STORE_SUC % (str(json_obj),colName))
        return True

    # Gets action message to respective pic
    def retrieve(self, criteria, colName):
        self.sendToStatus(db.INFO_DB_RETR_ATT % colName)
        # Find data in given collection matching specified criteria
        col = self.database[colName]
        doc = col.find_one(criteria)
        if doc:
            del doc['_id']
            self.sendToStatus(db.INFO_DB_RETR_SUC % colName)
        else:
            raise sendToStatus(db.ERROR_DB_RETR % colName)
        return doc