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
#include <cstring>
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
    m_commInterface = new SerialInterface(std::bind(msgReceiver, this, std::placeholders::_1, std::placeholders::_2), RECEIVE_BUFFER_WIDTH);
    // Initialize the output buffer for the debug messages
    m_msgRingBuffer = new Ringbuffer<uint8_t>(MSG_BUFFER_SIZE, MSG_BUFFER_WIDTH);
    m_msgBuf        = new uint8_t [MSG_BUFFER_WIDTH];

}

/*******************************************************************************//***
* \brief Destructor
*
************************************************************************************/
DebugMessages::~DebugMessages()
{
    delete m_commInterface;
    delete m_msgRingBuffer;
    delete m_msgBuf;
}

/*******************************************************************************//***
* \brief Message receiver
*
* Callback for the lower level receiving routine.
*
* @param    buffer  Result of the receive operation.
* @param    size    Size in bytes of the received data.
*
************************************************************************************/
void DebugMessages::msgReceiver(uint8_t *buffer, uint32_t size)
{
    // uint8_t lastData = buffer[size - 1];

    // Loop through the received dataset to find concluded debug messages
    for (uint32_t i = 0; i < size; i++ )
    {
        m_msgBuf[m_actualBufferIdx] = buffer[i];
        
        if (m_actualBufferIdx < MSG_BUFFER_WIDTH)
            m_actualBufferIdx++;

        if (buffer[i] == '\n')
        {
            // Copy data and reset the buffer
            m_msgRingBuffer->PutElement(m_msgBuf);
            m_actualBufferIdx = 0;
            // Overwrite the previously received data
            std::memset(m_msgBuf, 0,(size_t)MSG_BUFFER_WIDTH);
        }
    }

}

/*******************************************************************************//***
* \brief Message receiver
*
* Gets called by the lower level receiving routine and manages the ringbuffer.
*
************************************************************************************/
void DebugMessages::msgStateMachine(void)
{
    // Debug message management is easy, since its just a simplex data connection
    if (this->m_commInterface->m_ifState == SERIAL_INTERFACE_IDLE)
        // Restart the interface to receive more data
        this->m_commInterface->receiveTask(RECEIVE_BUFFER_WIDTH, DEBUG_MSG_BYTE_TO_BYTE_TIMEOUT_MS);

}

