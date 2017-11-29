# Database fields and globals
host        = '127.0.0.1'
port        = 27017
dbname      = 'team14'
deliv_nav   = 'deliv_nav'
deliv_sense = 'deliv_sense'
scan_nav    = 'scan_nav'
scan_sense  = 'scan_sense'
gui         = 'gui'

# Database initialization messages
INFO_DB_CONN_ATT            = 'Attempting to connect to database.' 
INFO_DB_CONN_SUC            = 'Connected to database on %s:%i'
INFO_DB_INIT                = 'Initializing database fields... '
INFO_DB_INIT_DELIV_NAV      = 'Initializing delivery navigation pic collection.'
INFO_DB_INIT_DELIV_SENSE    = 'Initializing delivery sensing pic collection.'
INFO_DB_INIT_SCAN_NAV       = 'Initializing scanner navigation pic collection.'
INFO_DB_INIT_SCAN_SENSE     = 'Initializing scanner sensing pic collection.'

# Database store and retrieve messages
INFO_DB_STORE_ATT           = 'Attempting to store data in %s'
INFO_DB_STORE_SUC           = 'Succesfully stored \"%s\" in \"%s\"'
INFO_DB_RETR_ATT            = 'Attempting to retrieve data from %s'
INFO_DB_RETR_SUC            = 'Succesfully retrieved data from %s'

# Database error messages
ERROR_DB_CONN               = 'ERROR: Database is not connected! (Is mongo server running?)'
ERROR_DB_STORE              = 'ERROR: Database could not store data in %s'
ERROR_DB_RETR               = 'ERROR: Database could not retrieve data from %s'
ERROR_DB_JSON               = 'ERROR: Data being stored is not a JSON object!'

class DelivNavFields:
    """
    "{\"SEQ\":%i,\"DELIV_NAV\":{ \"X\":%u, \"Y\":%u, \"OR\":%u }}!"
    "{\"SEQ\":%i,\"DELIV_NAV\":{\"STATUS\":0,\"MSG\":\"Rover is IDLE, requesting task\"}}!"
    "{\"SEQ\":%i,\"DELIV_NAV\":{\"TASK\":\"RECIEVED\"}}!"
    "{\"SEQ\":%i,\"DELIV_NAV\":{\"ACTION\":\"RECIEVED\"}}!"
    "{\"SEQ\":%i,\"DELIV_NAV\":{\"SET_MAGNET\":%u, \"IR_DATA\":%u}}!"
    "{\"SEQ\":%i,\"DELIV_NAV\":{\"STATE\":%u, \"IR\":%u, \"MAG\":%u}}!"
    """
    col_name    = deliv_nav
    # master token for incoming JSON
    token       = "DELIV_NAV"
    # tokens to find which message type
    tok_pos     = "X"
    tok_status  = "STATUS"
    tok_mag     = "SET_MAGNET"
    tok_state   = "STATE"
    # search criteria dicts
    crit_pos    = "POS"
    crit_status = "STATUS"
    crit_mag    = 'SET_MAG'
    crit_state  = "STATE"

class DelivSenseFields:
    """
    "{\"SEQ\":%i,\"DELIV_SENSE\": { \"MAGNET\":%i}}!"
    "{\"SEQ\":%i,\"DELIV_SENSE\":{\"IRDIST\":%i,\"OBJ\":%i}}!"
	"{\"SEQ\":%i,\"DELIV_SENSE\":{\"IRDIST\":%i,\"OBJ\":%i}}!"
    """
    col_name    = deliv_sense
    # master token for incoming JSON
    token       = "DELIV_SENSE"
    # tokens to find which message type
    tok_mag     = "MAGNET"
    tok_ir      = "IRDIST"
    tok_obj     = "OBJ"
    # search criteria dicts
    crit_mag     = "MAGNET"
    crit_ir      = "IRDIST"
    crit_obj     = "OBJ"

class ScanNavFields:
    """
    "{\"SEQ\":%i,\"SCAN_NAV\":{\"STATUS\":1,\"MSG\":\"Missing sequence number, expected %u got %u\"}}!"
    "{\"SEQ\":%i,\"SCAN_NAV\":{\"STATUS\":1,\"MSG\":\"Bad JSON message format\"}}!"
    "{\"SEQ\":%i,\"SCAN_NAV\": { \"DIST\": %u, \"STATE\": \"%s\" }}!"
    """
    col_name    = scan_nav
    # master token for incoming JSON
    token       = "SCAN_NAV"
    # tokens to find which message type
    tok_dist    = "DIST"
    tok_state   = "STATE"
    # search criteria dicts
    crit_dist   = "DIST"
    crit_state  = "STATE"

class ScanSenseFields:
    """
    "{\"SEQ\":%i,\"SCAN_SENSE\":{\"ZONE\":%i,\"RED\":%i,\"GREEN\":%i,\"BLUE\":%i,\"MSGTYPE\":\"%s\"}}!"
    """
    col_name    = scan_sense
    # master token for incoming JSON
    token       = "SCAN_SENSE"
    # tokens to find which message type
    tok_zone    = "ZONE"
    tok_r       = "RED"
    tok_g       = "GREEN"
    tok_b       = "BLUE"
    top_type    = "MSGTYPE"
    # search criteria dicts
    crit_zone_a = 'zone_a'
    crit_zone_b = 'zone_b'
    crit_zone_c = 'zone_c'

class GuiFields:
    col_name = gui
    # criteria
    crit_zone_a = ScanSenseFields.crit_zone_a
    crit_zone_b = ScanSenseFields.crit_zone_b
    crit_zone_c = ScanSenseFields.crit_zone_c