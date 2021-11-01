from RobComInterface import RobComInterface as RC
from RobComInterface import DebugMessage
from time import sleep

i = 0
IF = RC()
debugHdl = IF.openConnection(DebugMessage, portNo=5, baudrate=115384)

while (i < 1000):
   
    message = IF.getDebugMessages(debugHdl)
    if message is not None:
        print (message)
        i += 1
