from pymongo import MongoClient
import pprint
connection = MongoClient()
db = connection.mag_database.mag_collection
while True:
    b = db.find_one()
    print(b['state'])
