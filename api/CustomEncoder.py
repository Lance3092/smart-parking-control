from BayStatus import BayState
import json 

class EnumEncoder(json.JSONEncoder) : 

    def default(self, obj):
        if type(obj) in BayState.values():
            return {"bayStatus": str(obj)}
        return json.JSONEncoder.default(self, obj)

    def as_enum(d):
        if "bayStatus" in d:
            name, member = d["bayStatus"].split(".")
            return getattr(BayState[name], member)
        else:
            return d