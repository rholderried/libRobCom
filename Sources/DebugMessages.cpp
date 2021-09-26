/********************************************************************************//**
 * \file DebugMessages.cpp
 * \author Roman Holderried
 *
 * \brief Handling of incoming debug messages. 
 *
 * <b> History </b>
 *      - 2021-09-26 - File creation. 
 *                     
 ***********************************************************************************/

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include <iostream>
#include <cstdint>
#include <cstdbool>
#include "DebugMessages.h"
#include "SerialInterface.h"

/************************************************************************************
 * Function definitions
 ***********************************************************************************/

/*******************************************************************************//***
* \brief DefCon
*
* Initializes a DebugMessages instance
*
************************************************************************************/
DebugMessages::DebugMessages()
{
    m_commInterface = new SerialInterface(std::bind(msgReceiver, this, std::placeholders::_1, std::placeholders::_2));
}

/*******************************************************************************//***
* \brief Destructor
*
************************************************************************************/
DebugMessages::~DebugMessages()
{
    delete m_commInterface;
}

/*******************************************************************************//***
* \brief Message receiver
*
* Gets called by the lower level receiving routine and manages the ringbuffer.
*
************************************************************************************/
void DebugMessages::msgReceiver(uint8_t *buffer, uint32_t size)
{


}

/*******************************************************************************//***
* \brief Message receiver
*
* Gets called by the lower level receiving routine and manages the ringbuffer.
*
************************************************************************************/
void DebugMessages::msgStateMachine(void)
{
    

}

