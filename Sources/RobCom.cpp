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
#include "DebugMessages.h"

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
* \brief Setup a serial connection to the RobCom device
************************************************************************************/
bool RobCom::establishSerialConnection(tSERIALCOMTYPE comType, uint8_t portNo, uint32_t baudrate)
{
    switch(comType)
    {
        case COMTYPE_DEBUGMESSAGES:
            m_debugMessages = new DebugMessages();
            m_debugMessages->establishConnection(portNo, baudrate);
            break;

        default:
            break;
    }
}

/*******************************************************************************//***
* \brief Get data from the debug interface
************************************************************************************/
uint32_t RobCom::getDebugMsg(DebugMessages *pInst, uint8_t** dataPtr)
{
    uint32_t hadElements = uint32_t(pInst->m_msgRingBuffer->GetElement(dataPtr));

    return (hadElements * DBGMSG_BUFFER_WIDTH);
}