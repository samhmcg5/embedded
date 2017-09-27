from collections import namedtuple

"""
JSON FORMAT:
{
    "origin":int,
    "type":int,
    "subject":int,
    "message":"some string ..."
}
"""

#################################
######  DEFINE CONSTANTS  #######
#################################

# # origin
# ORIG_DELIVNAV    = 0
# ORIG_DELIVSENSE  = 1
# ORIG_SCANNAV     = 2
# ORIG_SCANSENSE   = 3

# # m_types
# T_ERROR   = -1
# T_UPDATE  = 0
# T_REQUEST = 1
# T_OTHER   = 2
# T_STATUS  = 3

# # subj
# S_NONE        = 0
# S_BAD_SEQ     = 1
# S_BAD_JSON    = 2
# S_CLIENT_DISCONN = 3
# S_CAUGHT_EXCPT = 4

# to parse into json...
STATUS_JSON = "{ \"origin\":%i, \"mtype\":%i, \"subj\":%i, \"text\":%s }!"

# struct to add to the queue to pass between threads
StatusMsg = namedtuple('StatusMsg', ['origin', 'mtype', 'subj', 'text'])

def msgToString(msg):
    return "[%s-%s]: %s"%(msg.origin, msg.mtype, msg.text)