"""
Interface to the RobCom library
"""
import os,sys
import ctypes
import numpy as np

class DebugMessage:
    def __init__(self, connectionType, portNumber, baudrate, interfaceNumber = 0):
        self.portNumber         = portNumber
        self.baudrate           = baudrate
        self.connectionType     = connectionType
        self.interfaceNumber    = interfaceNumber


class RobComInterface:

    DEFAULT_DLL_PATH = os.path.join(os.path.dirname(os.path.dirname(__file__)),'Release','RobCom.dll')

    def __init__(self, libPath = DEFAULT_DLL_PATH):
        """
        Initializes the RobCom interface

        Parameters:
        -----------
            - Path to the RobCom driver library
        """

        RobComHdl      = ctypes.POINTER(ctypes.c_char)
        dbgMsgPtr      = np.ctypeslib.ndpointer(dtype=np.uint8, ndim=1, shape=(1024,), flags='C_CONTIGUOUS')

        os.add_dll_directory(os.path.dirname(libPath))
        self.lib = ctypes.CDLL(libPath, winmode=0)

#        self.lib.RobComInit.argtypes        = []
        self.lib.RobComInit.restype         = RobComHdl
        self.lib.debugMsgInterface.argtypes = [RobComHdl, ctypes.c_uint, ctypes.c_uint]
        self.lib.getDebugMsg.argtypes       = [RobComHdl]
        self.lib.getDebugMsg.restype        = dbgMsgPtr
        self.lib.deleteRobCom.argtypes      = [RobComHdl]

        self.RobComInstance = self.lib.RobComInit() # Initialize a RobCom type object
    
    def __del__(self):

         self.lib.deleteRobCom(self.RobComInstance)



    def closeRobCom(self):
        pass
       


    def openConnection(self, object, **kwargs):
        """
        Establishes the connection to the RobCom device.

        Parameters:
        -----------
            - Interface description object
            - Interface parameters
        """

        connectionData = None 
        
        if object.__name__ == "DebugMessage":
            portNo = kwargs["portNo"]
            baudrate = kwargs["baudrate"]
            self.lib.debugMsgInterface(self.RobComInstance, portNo, baudrate)

            connectionData = object(object.__name__, portNo, baudrate)

        return connectionData


    def getDebugMessages(self, object):
        """
        Polls the message buffer and returns the debug message if there
        has one been received.

        Parameters:
        -----------
            - Interface description object
        """

        decoded = None

        try:
            message = self.lib.getDebugMsg(self.RobComInstance)
            splitted = np.split(message, np.where(message==10)[0])[0]
            decoded = splitted.tostring().decode("ascii")
        except:
            pass
    
        return decoded






        



        