/********************************************************************************//**
* \file RobCom.h
* \author Roman Holderried
*
* \brief Defines and typedefs supporting dll shared library construction.
*
* <b> History </b>
*      - 2021-09-30 - File creation. 
*                     
***********************************************************************************/

#ifndef ROBCOM_H_
#define ROBCOM_H_


/***********************************************************************************
* Includes
***********************************************************************************/
#include <iostream>
#include <cstdint>
#include <cstdbool>
#include "SerialMessageHandler.h"
//#include "RobComUSB.h"
#include "RobComSerial.h"
#include "CommonTypes.h"
/***********************************************************************************
* Defines
***********************************************************************************/
#define DEFAULT_DEBUG_MESSAGES_BUFFER_WIDTH     1024
#define DEFAULT_DEBUG_MESSAGES_BUFFER_SIZE      10
#define DEBUG_MESSAGES_TERMINATION_SYMBOL       '\n'
#define RSMSG_FRAME_LENGTH                      64
#define RSMSG_BUFFER_WIDTH                      RSMSG_FRAME_LENGTH + 5
#define RSMSG_BUFFER_SIZE                       10
#define DEFAULT_RECEIVE_TIMEOUT_MS              200
/***********************************************************************************
* Typedefs
***********************************************************************************/
typedef enum
{
    COMTYPE_DEBUGMESSAGES,
    COMTYPE_ROBCOMSERIAL
}tSERIALCOMTYPE;


/***********************************************************************************
* Classes
***********************************************************************************/
class RobCom
{
    public:
        SerialMessageHandler    *m_debugMessages = nullptr;
        RobComSerial            *m_robComSerial = nullptr;

        tMESSAGE                m_request = tMESSAGE_DEFAULTS;
        tMESSAGE                m_response = tMESSAGE_DEFAULTS;

        RobCom();
        ~RobCom();
        
        bool establishSerialConnection( tSERIALCOMTYPE comType, 
                                        uint8_t portNo, 
                                        uint32_t baudrate, 
                                        uint32_t bufLen = 0, 
                                        uint32_t receiveTimeout_ms = DEFAULT_RECEIVE_TIMEOUT_MS,
                                        tMESSAGE *request = nullptr, tMESSAGE *response = nullptr);

        //bool establishUSBConnection(COMCONFIG usbConfig);
        uint32_t getDebugMsg(uint8_t* dataPtr);
        //int32_t getRobComSerialMsg(RobComUSB *pInst, uint8_t** dataPtr);

};

#endif // ROBCOM_H_