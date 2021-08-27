from enum import Enum 
from enum import IntEnum

class BayState(IntEnum) :
    UNINITIALIZED = 0 
    OPEN = 1 
    BOOKED = 2 
    TAKEN = 4 