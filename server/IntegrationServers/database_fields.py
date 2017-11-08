# Database fields and globals
host = '127.0.0.1'
port = 27017
dbname = 'team14'
deliv_nav = 'deliv_nav'
deliv_sense = 'deliv_sense'
scan_nav = 'scan_nav'
scan_sense = 'scan_sense'

# Database initialization messages
INFO_DB_CONN_ATT        	= 'Attempting to connect to database.' 
INFO_DB_CONN_SUC            = 'Connected to database on... '
INFO_DB_INIT            	= 'Initializing database fields... '
INFO_DB_INIT_DELIV_NAV    	= 'Initializing delivery navigation pic collection.'
INFO_DB_INIT_DELIV_SENSE    = 'Initializing delivery sensing pic collection.'
INFO_DB_INIT_SCAN_NAV    	= 'Initializing scanner navigation pic collection.'
INFO_DB_INIT_SCAN_SENSE     = 'Initializing scanner sensing pic collection.'

# Database store and retrieve messages
INFO_DB_STORE_ATT       	= 'Attempting to store data in... '
INFO_DB_STORE_SUC       	= 'Succesfully stored \"%s\" in \"%s\"'
INFO_DB_RETR_ATT			= 'Attempting to retrieve data from... '
INFO_DB_RETR_SUC			= 'Succesfully retrieved data from... '

# Database error messages
ERROR_DB_CONN           	= 'Database is not connected! (Is mongo server running?)'
ERROR_DB_STORE				= 'Database could not store data in... '
ERROR_DB_RETR				= 'Database could not retrieve data from... '
ERROR_DB_JSON				= 'Data being stored is not a JSON object!'

# Default collection criteria for retrieves
# ALL COLLECTIONS
DEFAULT_CRITERIA = '{ "ACTION": { "$exists": true } }'

# DELIV NAV

# DELIV SENSE

# SCAN NAV

# SCAN SENSE
QUOTA_CRITERIA = '{ "QUOTA": { "$exists": true } }'

# Default collection storage fields
# DELIV NAV
DELIV_NAV_DEFAULT_STATUS = '{ "SEQ": 0, "DELIV_NAV": { "STATUS": 0, "MSG": "Rover is IDLE, requesting task" } }'
DELIV_NAV_TASK_ACK       = '{ "SEQ": 0, "DELIV_NAV": { "TASK": "RECEIVED" } }'
DELIV_NAV_ACTION_ACK     = '{ "SEQ": 0, "DELIV_NAV": { "ACTION": "RECEIVED" } }'
DELIV_NAV_DEFAULT_REQ    = '{ "SEQ": 0, "DELIV_NAV": { "SET_MAGNET": 0, "IR_DATA": 0 } }'
DELIV_NAV_DEFAULT_POS    = '{ "SEQ": 0, "DELIV_NAV": { "X": 0, "Y": 20, "OR": 0 } }'
DELIV_NAV_DEFAULT_ERR    = '{ "SEQ": 0, "DELIV_NAV": { "ERROR": 0, "MSG": "message" } }'
DELIV_NAV_FIELDS = [DELIV_NAV_DEFAULT_STATUS,
                    DELIV_NAV_TASK_ACK,
                    DELIV_NAV_ACTION_ACK,
                    DELIV_NAV_DEFAULT_REQ,
                    DELIV_NAV_DEFAULT_POS,
                    DELIV_NAV_DEFAULT_ERR]

# DELIV SENSE
DELIV_SENSE_DEFAULT_ACTION = '{ "SEQ": 0, "ACTION": 0 }'
DELIV_SENSE_FIELDS = [ DELIV_SENSE_DEFAULT_ACTION ]

# SCAN NAV
SCAN_NAV_DEFAULT_ACTION = '{ "SEQ": 0, "ACTION": 0 }'
SCAN_NAV_FIELDS = [ SCAN_NAV_DEFAULT_ACTION ]

# SCAN SENSE
SCAN_SENSE_DEFAULT_ZONE_ACTION 	= '{ "SEQ": 0, "ZONE": 1, "ACTION": 0 }' 
SCAN_SENSE_DEFAULT_ZONE_1_CURR 	= '{ "SEQ": 0, "SCAN_SENSE":{ "ZONE": 1, "RED": 0, "GREEN": 0, "BLUE": 0 } }'
SCAN_SENSE_DEFAULT_ZONE_2_CURR 	= '{ "SEQ": 0, "SCAN_SENSE":{ "ZONE": 2, "RED": 0, "GREEN": 0, "BLUE": 0 } }'
SCAN_SENSE_DEFAULT_ZONE_3_CURR 	= '{ "SEQ": 0, "SCAN_SENSE":{ "ZONE": 3, "RED": 0, "GREEN": 0, "BLUE": 0 } }'
SCAN_SENSE_DEFAULT_ZONE_1_QUOTA = '{ "SEQ": 0, "QUOTA":{ "ZONE": 1, "RED": 0, "GREEN": 0, "BLUE": 0 } }'
SCAN_SENSE_DEFAULT_ZONE_2_QUOTA = '{ "SEQ": 0, "QUOTA":{ "ZONE": 2, "RED": 0, "GREEN": 0, "BLUE": 0 } }'
SCAN_SENSE_DEFAULT_ZONE_3_QUOTA = '{ "SEQ": 0, "QUOTA":{ "ZONE": 3, "RED": 0, "GREEN": 0, "BLUE": 0 } }'
SCAN_SENSE_FIELDS = [ SCAN_SENSE_DEFAULT_ZONE_ACTION, 
					SCAN_SENSE_DEFAULT_ZONE_1_CURR, 
					SCAN_SENSE_DEFAULT_ZONE_2_CURR, 
					SCAN_SENSE_DEFAULT_ZONE_3_CURR,
					SCAN_SENSE_DEFAULT_ZONE_1_QUOTA,
					SCAN_SENSE_DEFAULT_ZONE_2_QUOTA,
					SCAN_SENSE_DEFAULT_ZONE_3_QUOTA ]

