import unittest
# Server defs
import server_defs as defs
import json
# Database
import database as db
import database_fields as dbf
# Server
import server as srv
import server_fields as srvf
# Client
import client as cli

# TODO
# Come up with more good and bad json examples
json_good = '{ "json": 0 }'
json_bad = '{ not json: 0 }'

# Store and Retrieve json objs
scan_sense_store_str = '{ "SEQ": 1, "SCAN_SENSE":{ "ZONE": 2, "RED": 1, "GREEN": 2, "BLUE": 3 } }'

# Switch to test database connectivity functionality
DBONLINE = True

# Server definition tests
# PASSED
# TODO (NONOPERATIONAL && NOT PASSED)
# Checksum
# Sequence
# Message Rates
class TestServerDefs(unittest.TestCase):

	def test_isjson(self):
		self.assertTrue(defs.isjson(json_good))
		self.assertFalse(defs.isjson(json_bad))		

# Database setup
class SimpleDatabaseTestCase(unittest.TestCase):
	
	def setUp(self):
		self.db = db.Database()

# Database tests
# PASSED
class TestDatabaseCalls(SimpleDatabaseTestCase):

	# Test database class initialization
	def test_init(self):
		self.assertIsInstance(self.db, type(db.Database()))
		self.assertEquals(self.db.host, dbf.localhost)
		self.assertEquals(self.db.port, dbf.port)
		
		self.assertEquals(self.db.dbname, dbf.dbname)
		self.assertEquals(self.db.deliv_nav, dbf.deliv_nav)
		self.assertEquals(self.db.deliv_sense, dbf.deliv_sense)
		self.assertEquals(self.db.scan_nav, dbf.scan_nav)
		self.assertEquals(self.db.scan_sense, dbf.scan_sense)
		
		self.assertIsNone(self.db.mongoclient)
		self.assertFalse(self.db.dbonline)
		self.assertIsNone(self.db.db)
		self.assertIsNone(self.db.deliv_nav_col)
		self.assertIsNone(self.db.deliv_sense_col)
		self.assertIsNone(self.db.scan_nav_col)
		self.assertIsNone(self.db.scan_sense_col)

	# Tests database initialization and clean
	@unittest.skipIf(DBONLINE is False, "db is offline, only test when up")
	def test_db_init(self):
		self.db.connect()
		self.db.db_init()

		self.assertIsNotNone(self.db.db)
		self.assertIsNotNone(self.db.deliv_nav_col)
		self.assertIsNotNone(self.db.deliv_sense_col)
		self.assertIsNotNone(self.db.scan_nav_col)
		self.assertIsNotNone(self.db.scan_sense_col)

		self.assertEquals(self.db.db.name, dbf.dbname)

		# Confirm collections are valid
		deliv_nav = self.db.db.validate_collection(self.db.deliv_nav_col)
		self.assertTrue(deliv_nav['valid'])
		
		deliv_sense = self.db.db.validate_collection(self.db.deliv_sense_col)
		self.assertTrue(deliv_sense['valid'])
		
		scan_nav = self.db.db.validate_collection(self.db.scan_nav_col)
		self.assertTrue(scan_nav['valid'])
		
		scan_sense = self.db.db.validate_collection(self.db.scan_sense_col)
		self.assertTrue(scan_sense['valid'])

		# Call db_init() again and confirm that items are only inserted once
		self.db.db_init()
		deliv_nav = self.db.db.validate_collection(self.db.deliv_nav_col)
		self.assertEquals(deliv_nav['nrecords'], len(dbf.DELIV_NAV_FIELDS))
		
		deliv_sense = self.db.db.validate_collection(self.db.deliv_sense_col)
		self.assertEquals(deliv_sense['nrecords'], len(dbf.DELIV_SENSE_FIELDS))
		
		scan_nav = self.db.db.validate_collection(self.db.scan_nav_col)
		self.assertEquals(scan_nav['nrecords'], len(dbf.SCAN_NAV_FIELDS))
		
		scan_sense = self.db.db.validate_collection(self.db.scan_sense_col)
		self.assertEquals(scan_sense['nrecords'], len(dbf.SCAN_SENSE_FIELDS))


		self.db.clean()
		self.assertIsNone(self.db.db)
		self.assertIsNone(self.db.deliv_nav_col)
		self.assertIsNone(self.db.deliv_sense_col)
		self.assertIsNone(self.db.scan_nav_col)
		self.assertIsNone(self.db.scan_sense_col)

	# Test store and retrieve functionality of database
	@unittest.skipIf(DBONLINE is False, "db is offline, only test when up")	
	def test_store_and_retrieve(self):
		self.db.connect()
		self.db.db_init()
		jsonobj = json.loads(scan_sense_store_str)
		
		seq = jsonobj['SEQ']
		zone = jsonobj['SCAN_SENSE']['ZONE']
		red = jsonobj['SCAN_SENSE']['RED']
		green = jsonobj['SCAN_SENSE']['GREEN']
		blue = jsonobj['SCAN_SENSE']['BLUE']

		self.assertEquals(seq, 1)
		self.assertEquals(zone, 2)
		self.assertEquals(red, 1)
		self.assertEquals(green, 2)
		self.assertEquals(blue, 3)

		res = self.db.store(self.db.scan_sense_col, '{ "SCAN_SENSE.ZONE": ' + str(zone) + '}', jsonobj, dbf.scan_sense)
		self.assertTrue(res) # result successfully stored
		doc = self.db.retrieve(self.db.scan_sense_col, '{ "SCAN_SENSE.ZONE": { "$exists": true, "$eq": ' + str(zone) + '} }', dbf.scan_sense)
		
		self.assertEquals(doc['SEQ'], seq)
		self.assertEquals(doc['SCAN_SENSE']['ZONE'], zone)
		self.assertEquals(doc['SCAN_SENSE']['RED'], red)
		self.assertEquals(doc['SCAN_SENSE']['GREEN'], green)
		self.assertEquals(doc['SCAN_SENSE']['BLUE'], blue)

		self.db.clean()
	
	# Test if database instance properly connects
	@unittest.skipIf(DBONLINE is False, "db is offline, only test when up")	
	def test_connect(self):
		self.assertFalse(self.db.isDBOnline())
		self.db.connect()
		self.assertTrue(self.db.isDBOnline())
	
	# Test if correct error is thrown when database doesnt conenct
	@unittest.skipIf(DBONLINE is True, "db is online, only test when off")
	def test_connect_err(self):
		self.assertFalse(self.db.isDBOnline())
		with self.assertRaises(ConnectionError):
			self.db.connect()
		self.assertFalse(self.db.isDBOnline())

	# Test if dbonline field is properly updated in Database class
	@unittest.skipIf(DBONLINE is False, "db is offline, only test when on")	
	def test_isdbonline(self):
		# On a connect 
		self.assertFalse(self.db.isDBOnline())
		self.db.connect()
		self.assertTrue(self.db.isDBOnline())

		# On a disconnect
		self.assertTrue(self.db.isDBOnline())
		self.db.disconnect()
		self.assertFalse(self.db.isDBOnline())

# Server setup
class SimpleServerTestCase(unittest.TestCase):
	
	def setUp(self):
		self.srv = srv.Server(srvf.IP_ADDR, srvf.scan_sense_port)
		self.cli = cli.Client(srvf.IP_ADDR, srvf.scan_sense_port)

# Server tests
# PASSED
class TestServerCalls(SimpleServerTestCase):

	def test_init(self):
		self.assertIsInstance(self.srv, type(srv.Server(srvf.IP_ADDR, srvf.scan_sense_port)))
		self.assertIsInstance(self.cli, type(cli.Client(srvf.IP_ADDR, srvf.scan_sense_port)))
		self.assertIsInstance(self.srv.db, type(db.Database()))

		self.assertEquals(self.srv.addr, srvf.IP_ADDR)
		self.assertEquals(self.srv.port, srvf.scan_sense_port)
		self.assertEquals(self.srv.size, 1)
		self.assertEquals(self.srv.backlog, 1)

		self.assertIsNone(self.srv.client)
		self.assertIsNone(self.srv.client_addr)

		self.assertIsNone(self.srv.socket)
		self.assertFalse(self.srv.srvonline)
		self.srv.reset()

	def test_socket_init(self):
		self.srv.socket_init()
		self.assertIsNotNone(self.srv.socket)
		self.srv.reset()

	def test_start(self):
		self.srv.socket_init()
		self.srv.start()
		self.assertTrue(True)
		self.srv.reset()

	# Tests connect, isSRVOnline, and disconnect
	def test_connect(self):
		self.srv.socket_init()
		self.srv.start()
		
		self.assertFalse(self.srv.isSRVOnline())

		self.srv.connect()
		
		self.assertIsNotNone(self.srv.client)
		self.assertIsNotNone(self.srv.client_addr)
		self.assertTrue(self.srv.isSRVOnline())

		self.srv.disconnect()
		self.assertFalse(self.srv.isSRVOnline())
		self.srv.reset()

	def test_send_recv_msg(self):
		self.srv.socket_init()
		self.srv.start()
		self.srv.connect()
		
		self.srv.sendmsg("Hello!")
		buf = ""
		while '!' not in buf:
			buf += self.srv.recvmsg()

		self.assertEquals(buf, "Hello!")
		buf = ""

		self.srv.disconnect()
		self.srv.reset()

if __name__ == '__main__':
	suite = unittest.TestSuite()
	# Server Definition Tests
	suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestServerDefs))
	# Database Tests
	suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestDatabaseCalls))
	# Server Tests
	suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestServerCalls))	
	
	# Run Tests
	unittest.TextTestRunner(verbosity=2).run(suite)