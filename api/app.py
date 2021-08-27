from enum import Enum
from flask import Flask
from flask import request;
from flask_restful import Resource, Api
from Bay import bay
from BayStatus import BayState
from BayStatusHandler import BayStatusHandler
from CustomEncoder import EnumEncoder 
import json

app = Flask(__name__)
api = Api(app)
bayhandler = BayStatusHandler()
class HelloWorld(Resource):
    def get(self):
        return {'hello': 'world'}
#for Setup of arduino controller, returns intial value for LED light 
@app.route('/setup/<bayid>',methods=['GET'])
def setup_bay_controller(bayid):
    reqAddr = str(request.remote_addr)

    reqBay = bayhandler.setupBay(bayid,reqAddr)

    return {
        'bayId' :f'{reqBay.bayID}',
        'bayStatus': f'{reqBay.status}'
    
    }
    
#Set state of parking controller based off external Data
@app.route('/setBayStatus/<bayid>',methods=['POST'])
def set_parking_status(bayid) :
    json_data = request.json
    bayStatus = json_data["bayStatus"]
    newbay = bayhandler.setBayStatus(bayid,bayStatus)
    return {'bay' :f'{bayid}',
    'bayStatus' : f'{newbay.status}'
    }

#Get current State of parking bay 
@app.route('/getBayStatus/<bayid>',methods=['GET']) 
def get_parking_status(bayid) : 
    newbay = bayhandler.setBayStatus(bayid)
    return {'bay' :f'{bayid}',
    'bayStatus' : f'{newbay.status}'
    }
#Todo Send PostRequest to backend 
@app.route('/updateParkingState/<bayid>',methods=['POST'])
def update_parking_status(bayid) :
    json_data = request.json
    bayState = int(json_data["bayState"])
    if (bayState) :
        bayStatus =BayState(1) 
    else :
        bayStatus = BayState(4)
    newbay = bayhandler.updateBayStatus(bayid,bayStatus)
    return {'bay' :f'{bayid}',
    'bayStatus' : f'{newbay.status}'
    }

api.add_resource(HelloWorld, '/')

if __name__ == '__main__':
    app.run(debug=True)