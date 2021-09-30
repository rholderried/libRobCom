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
    DEBUGMESSAGES,
    ROBCOMSERIAL,
    ROBCOMUSB
}tCONNECTIONTYPE;
/***********************************************************************************
* Classes
***********************************************************************************/
class RobCom
{
    public:
        DebugMessages m_debugMessages;

        RobCom();
        bool establishSerialConnection(tCONNECTIONTYPE connectionType, uint8_t portNo, uint32_t baudrate);
        bool establishUSBConnection(tCONNECTIONTYPE connectionType, COMCONFIG usbConfig);
        int32_t getData(DebugMessages *pInst, uint8_t** dataPtr);
        int32_t getData(RobComUSB *pInst, uint8_t** dataPtr);

};