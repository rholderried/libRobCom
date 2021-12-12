from RobComInterface import RobComInterface as RC
from RobComInterface import DebugMessage
from time import sleep

i = 0
IF = RC()
debugHdl = IF.openConnection(DebugMessage, portNo=3, baudrate=115384)

while (i < 10):
    
    message = IF.getDebugMessages(debugHdl)
    if message is not None:
        print (message)
        i += 1
