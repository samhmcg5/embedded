from collections import namedtuple

# to parse into json...
STATUS_JSON = "{\"origin\":\"%s\", \"mtype\":\"%s\", \"subj\":\"%s\", \"text\":\"%s\"}!"

# struct to add to the queue to pass between threads
StatusMsg = namedtuple('StatusMsg', ['origin', 'mtype', 'subj', 'text'])

def msgToString(msg):
    return "[%s-%s]: %s"%(msg.origin, msg.mtype, msg.text)