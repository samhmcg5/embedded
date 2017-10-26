import json

# Determines if a string is JSON format
def isjson(json_str):
    try:
        json_obj = json.loads(json_str)
    except ValueError:
        return False
    return True

