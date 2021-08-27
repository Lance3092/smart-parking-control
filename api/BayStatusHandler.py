import requests;
from Bay import bay
from BayStatus import BayState
import json
#Todo Add Requests 
bays = []
def findValueByID(bayID,bays) -> bay :
       for value in bays :
           if value.bayID == bayID:
               return value
       return None 
class BayStatusHandler :
    def setupBay(self,bayID,address) -> bay  :
        searchbay = findValueByID(bayID,bays)
        if searchbay is not None : 
            return searchbay 
        else : 
    
            newBay = bay(bayID,address,int(BayState.OPEN))
            bays.append(newBay)
            print(newBay)
            return newBay
            
    ## Todo define return type properly 
    def setBayStatus(self,bayId,State) -> bay : 
        print(len(bays))
        bay = findValueByID(bayId,bays) 
        if bay == None :
            return None 
        bay.status = State
        requestbody = {
            'bayStatus' : f'{bay.status}'
        }
        requestURL = "http://"+ bay.address + "/setStatus"
      
        request = requests.post(requestURL,json.dumps(requestbody))
        return bay
    
    def updateBayStatus(self,bayId,State) -> bay : 
        print(len(bays))
        bay = findValueByID(bayId,bays) 
        if bay == None :
            return None 
        bay.status = State
        return bay

    def getBayStatus(self,bayId) -> bay :
        bay = findValueByID(bayId,bays)  
        if bay == None :
            return None 
        return bay
    


    
