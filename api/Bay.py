class bay : 
    def __init__(self,bayID,address,status) :
        self.bayID = bayID
        self.address = address
        self.status = int(status)
        self.currentStatus = False 