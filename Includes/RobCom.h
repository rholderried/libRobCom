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
#include "DebugMessages.h"
#include "RobComUSB.h"
// #include "RobComSerial.h"
/***********************************************************************************
* Defines
***********************************************************************************/

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
        DebugMessages *m_debugMessages = nullptr;

        RobCom();
        ~RobCom();
        
        bool establishSerialConnection(tSERIALCOMTYPE comType, uint8_t portNo, uint32_t baudrate);
        //bool establishUSBConnection(COMCONFIG usbConfig);
        uint32_t getDebugMsg(DebugMessages *pInst, uint8_t** dataPtr);
        //int32_t getRobComSerialMsg(RobComUSB *pInst, uint8_t** dataPtr);

};

#endif // ROBCOM_H_