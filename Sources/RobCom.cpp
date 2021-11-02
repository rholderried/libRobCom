/********************************************************************************//**
 * \file RobCom.cpp
 * \author Roman Holderried
 *
 * \brief RobCom main class. 
 *
 * <b> History </b>
 *      - 2021-09-26 - File creation. 
 *                     
 ***********************************************************************************/


/***********************************************************************************
* Includes
***********************************************************************************/
#include <cstdint>
#include <cstdbool>
#include <iostream>
#include "RobCom.h"
#include "SerialMessageHandler.h"

/***********************************************************************************
* Defines
***********************************************************************************/

/*******************************************************************************//***
* \brief RobCom c'tor
************************************************************************************/
RobCom::RobCom()
{
    
}

/*******************************************************************************//***
* \brief RobCom d'tor
************************************************************************************/
RobCom::~RobCom()
{
    // Delete all the interfaces
    if (m_debugMessages != nullptr)
        delete m_debugMessages;
}

/*******************************************************************************//***
* \brief Setup a serial connection to the RobCom device
************************************************************************************/
bool RobCom::establishSerialConnection(tSERIALCOMTYPE comType, uint8_t portNo, uint32_t baudrate, uint32_t bufLen)
{
    bool successFlag = false;

    switch(comType)
    {
        case COMTYPE_DEBUGMESSAGES:

            if (bufLen == 0)
                bufLen = DEFAULT_DEBUG_MESSAGES_BUFFER_WIDTH;

            m_debugMessages = new SerialMessageHandler(bufLen, DEFAULT_DEBUG_MESSAGES_BUFFER_SIZE, DEBUG_MESSAGES_TERMINATION_SYMBOL);
            successFlag = m_debugMessages->establishConnection(portNo, baudrate);

            break;

        default:
            break;
    }

    return successFlag;
}

/*******************************************************************************//***
* \brief Get data from the debug interface
************************************************************************************/
uint32_t RobCom::getDebugMsg(SerialMessageHandler *pInst, uint8_t** dataPtr)
{
    uint32_t retrievedData = uint32_t(pInst->m_msgRingBuffer->GetElement(dataPtr));

    return retrievedData;
}