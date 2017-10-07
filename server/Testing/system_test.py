import unittest
import server_defs as defs
#import server_threads as threads
import database as db
import database_fields as dbf

# TODO
# Come up with more good and bad json examples
json_good = '{ "json": 0 }'
json_bad = '{ not json: 0 }'

# Switch to test database connectivity functionality
DBONLINE = True

# Server definition tests
class TestServerDefs(unittest.TestCase):

	def test_isjson(self):
		self.assertTrue(defs.isjson(json_good))
		self.assertFalse(defs.isjson(json_bad))


# Database setup
class SimpleDatabaseTestCase(unittest.TestCase):
	
	def setUp(self):
		self.db = db.Database()

# Database tests
class TestDatabaseCalls(SimpleDatabaseTestCase):

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

	# Tests database initialization
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

	# TODO
	#def test_store(self):
		
	# TODO		
	#def test_retrieve(self):
	
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


if __name__ == '__main__':
	suite = unittest.TestSuite()
	# Server Definition Tests
	suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestServerDefs))
	# Database Tests
	suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestDatabaseCalls))	
	
	# Run Tests
	unittest.TextTestRunner(verbosity=2).run(suite)