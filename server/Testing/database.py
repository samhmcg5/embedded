# Database class, constants, and definitions
from pymongo import MongoClient
from pymongo.errors import ServerSelectionTimeoutError
import database_fields as db
import server_defs as srv
import sys
import json

# TODO 
# handle disconnect
# send messages to status viewer and not just print

# NON OPERATIONAL
# NOT TESTED 
class Database():

	def __init__(self):
		# Connection settings
		self.host = db.localhost
		self.port = db.port
		
		# Database and collection names
		self.dbname = db.dbname
		self.deliv_nav = db.deliv_nav
		self.deliv_sense = db.deliv_sense
		self.scan_nav = db.scan_nav
		self.scan_sense = db.scan_sense
		
		# Database connection and collection handlers
		self.mongoclient = None
		self.dbonline = False
		self.db = None
		self.deliv_nav_col = None
		self.deliv_sense_col = None
		self.scan_nav_col = None
		self.scan_sense_col = None

	# Initialize database and collection fields
	def db_init(self):
		#self.print(db.INFO_DB_INIT)
		self.db = self.mongoclient[self.dbname]
		self.col_init()
	
	# Initialize collections with default storage fields
	def col_init(self):
		# Initialize delivery navigation
		#self.print(db.INFO_DB_INIT_DELIV_NAV)
		self.deliv_nav_col = self.db[self.deliv_nav]
		self.fill_default_fields(self.deliv_nav_col, db.DELIV_NAV_FIELDS, self.deliv_nav)

		# Initialize delivery sensing
		#self.print(db.INFO_DB_INIT_DELIV_SENSE)
		self.deliv_sense_col = self.db[self.deliv_sense]	
		self.fill_default_fields(self.deliv_sense_col, db.DELIV_SENSE_FIELDS, self.deliv_sense)
		
		# Initialize scanner navigation
		#self.print(db.INFO_DB_INIT_SCAN_NAV)
		self.scan_nav_col = self.db[self.scan_nav]
		self.fill_default_fields(self.scan_nav_col, db.SCAN_NAV_FIELDS, self.scan_nav)

		# Initialize scanner sensing
		#self.print(db.INFO_DB_INIT_SCAN_SENSE)
		self.scan_sense_col = self.db[self.scan_sense]
		self.fill_default_fields(self.scan_sense_col, db.SCAN_SENSE_FIELDS, self.scan_sense)

	# Fill collections with default fields
	def fill_default_fields(self, col, fields, colName):
		for jsonstr in fields:
			if srv.isjson(jsonstr):
				#self.print(db.INFO_DB_STORE_ATT + colName)
				res = col.replace_one(json.loads(jsonstr), json.loads(jsonstr), True)
				if res.modified_count == 1 or res.upserted_id is not None:
					self.print(db.INFO_DB_STORE_SUC + colName)
				# should never happen
				else: 
					raise RuntimeError(db.ERROR_DB_STORE + colName)
			else:
				raise ValueError(db.ERROR_DB_JSON)

	# Connect to database
	def connect(self):
		#self.print(db.INFO_DB_CONN_ATT)
		self.mongoclient = MongoClient(self.host, self.port, serverSelectionTimeoutMS=3000)
		try: 
			result = self.mongoclient.admin.command("ismaster")
		except ServerSelectionTimeoutError as err:
			self.dbonline = False
			raise ConnectionError(db.ERROR_DB_CONN)
		self.dbonline = True
		#self.print(db.INFO_DB_CONN_SUC + self.host + ":" + str(self.port))

	# TODO 
	def disconnect(self):
		# Disconnect mongo
		self.dbonline = False

	# Empty databse and clear fields
	def clean(self):
		self.mongoclient.drop_database(self.dbname)
		self.db = None
		self.deliv_nav_col = None
		self.deliv_sense_col = None
		self.scan_nav_col = None
		self.scan_sense_col = None

	# Check if database is connected
	def isDBOnline(self):
		return self.dbonline

	# Stores a JSON formatted object in the database
	# Return TRUE on successful store
	def store(self, col, criteria, json_obj, colName):
		#self.print(db.INFO_DB_STORE_ATT + colName)
		# Replace database field with given criteria with new json_obj data
		res = col.replace_one(criteria, json_obj)
    	# Criteria does not match database field in a given collection
		if res.matchedCount == 0:
			raise RuntimeError(db.ERROR_DB_STORE + colName)
		else:
			self.print(db.INFO_DB_STORE_SUC + colName)
		return True

	# Gets action message to respective pic
	def retrieve(self, col, criteria, colName):
		#self.print(db.INFO_DB_RETR_ATT + colName)
		# Find data in given collection matching specified criteria
		doc = col.find_one(criteria, '{ "_id": 0 }')
		if doc:
			self.print(db.INFO_DB_RETR_SUC + colName)
		# Data field in collection could not be found
		else:
			raise RuntimeError(db.ERROR_DB_RETR + colName)
		return doc

    # Override print so that it works on 
    # TODO Update to send to status viewer
	def print(self, msg):
		print(msg)
		sys.stdout.flush()