/********************************************************************************//**
 * \file RobComShared.c
 * \author Roman Holderried
 *
 * \brief Exported RobCom functions. 
 *
 * <b> History </b>
 *      - 2021-09-30 - File creation. 
 *                     
 ***********************************************************************************/
/***********************************************************************************
* Includes
***********************************************************************************/
#include "RobCom.h"
#include <cstdint>
#include <iostream>

/***********************************************************************************
* Defines
***********************************************************************************/
#define DLLEXPORT extern "C" __declspec(dllexport)

/*******************************************************************************//***
* \brief Returns an instance pointer of the RobCom interface.
************************************************************************************/
DLLEXPORT RobCom* RobComInit()
{
    return new RobCom();
}

/*******************************************************************************//***
* \brief Initializes a serial connection
************************************************************************************/
DLLEXPORT int debugMsgInterface(RobCom* pInst, unsigned int portNo, unsigned int baudrate)
{
    return pInst->establishSerialConnection(COMTYPE_DEBUGMESSAGES, portNo, baudrate);
}

/*******************************************************************************//***
* \brief Writes the messages into the buffer passed by the application
************************************************************************************/
DLLEXPORT int getDebugMsg(RobCom* pInst, unsigned char* bufferExt)
{
    // Do we really need that typecast or is it working just with uint8_t?
    uint8_t *bufPtr = reinterpret_cast<uint8_t*>(bufferExt);

    return pInst->getDebugMsg(pInst->m_debugMessages, &bufPtr);
}