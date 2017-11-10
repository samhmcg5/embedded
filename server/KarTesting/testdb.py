from pymongo import MongoClient
import pprint
connection = MongoClient()
db = connection.mag_database.mag_collection
state = {"state": 0}
db.insert(state)
while True:
    a = input()
    b = db.find_one()
    existing = b['state']
    db.update_one({
        '_id': b['_id']
        },{
            '$set': {
                "state": a
                }
            }, upsert=False)
    #print(b['state'])
