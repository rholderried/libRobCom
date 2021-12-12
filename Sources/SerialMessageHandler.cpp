/********************************************************************************//**
 * \file SerialMessageHandler.cpp
 * \author Roman Holderried
 *
 * \brief Handling of incoming serial messages. 
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
#include "SerialMessageHandler.h"
#include "SerialInterface.h"

/************************************************************************************
 * Function definitions
 ***********************************************************************************/

/*******************************************************************************//***
* \brief DefCon
*
* Initializes a SerialMessageHandler instance
*
* @param    bufWidth        Defines how long the message can be.
* @param    bufSize         Defines the message capacity of the ringbuffer
* @param    msgTermSymbol   Symbol for message termination, i.e. '\n'
*
************************************************************************************/
SerialMessageHandler::SerialMessageHandler(uint32_t bufWidth, uint32_t bufSize, uint8_t msgTermSymbol)
{
    m_commInterface = new SerialInterface(std::bind(msgReceiver, this, std::placeholders::_1, std::placeholders::_2), RECEIVE_BUFFER_WIDTH);
    // Initialize the output buffer for the debug messages
    m_msgRingBuffer = new Ringbuffer<uint8_t>(bufSize, bufWidth);
    m_msgBuf        = new uint8_t [bufWidth];
    m_bufWidth      = bufWidth;
    m_bufSize       = bufSize;
    m_msgTermSymbol = msgTermSymbol;
    m_receiveImmediately = false;
}

/*******************************************************************************//***
* \brief Destructor
************************************************************************************/
SerialMessageHandler::~SerialMessageHandler()
{
    // Delete the receiver thread
    if (m_connectionEstablished)
    {
        m_commInterface->m_receiveTaskRunning = false;
        // Thread will be joined in the d'tor of the Threadwrapper object
        delete m_eventHandlerPtr;
    }
    
    delete m_commInterface;
    delete m_msgRingBuffer;
    delete m_msgBuf;
}

/*******************************************************************************//***
* \brief Message receiver
*
* Callback for the lower level receiving routine. This routine is able to handle 
* debug messages which are sent directly consecutive (there was no time to process
* the previous message) by copying the received messages into a ringbuffer structure.
*
* @param    buffer  Result of the receive operation.
* @param    size    Size in bytes of the received data.
*
************************************************************************************/
void SerialMessageHandler::msgReceiver(uint8_t *buffer, uint32_t size)
{
    // uint8_t lastData = buffer[size - 1];

    // Loop through the received dataset to find concluded debug messages
    for (uint32_t i = 0; i < size; i++ )
    {
        m_msgBuf[m_actualBufferIdx] = buffer[i];
        
        if (m_actualBufferIdx < m_bufWidth)
            m_actualBufferIdx++;

        if (buffer[i] == m_msgTermSymbol)
        {
            // Copy data and reset the buffer
            m_msgRingBuffer->PutElement(m_msgBuf, size = i+1);
            m_actualBufferIdx = 0;
            // Overwrite the previously received data
            std::memset(m_msgBuf, 0,(size_t)m_bufWidth);

            // Call the external function, if there is one
            if (m_callbacks.onReceivedMsg != nullptr)
                m_callbacks.onReceivedMsg();

            // Eventually prevent serial interface from receiving more data
            if (!m_receiveImmediately)
                m_commInterface->m_readyToReceive = false;
        }
    }

}

/*******************************************************************************//***
* \brief Starts the receiving routine
*
* @param    enableReceiveState  Decides if the low level receiving routine shall be
*                               entered immediately. Shall be set to false for flow
*                               controlled communication.
* @returns true when the start of the receiver was successfull, false otherwise
************************************************************************************/
bool SerialMessageHandler::startReceiver(bool enableReceiveState)
{
    if (m_connectionEstablished == false)
        return false;
    else
    {
        m_commInterface->m_receiveTaskRunning = true;
        m_commInterface->m_readyToReceive = enableReceiveState;
        m_eventHandlerPtr = new ThreadWrapper(std::bind(SerialInterface::receiveTask, this->m_commInterface));
    }
    return true;
}

/*******************************************************************************//***
* \brief Starts the receiving routine
*
* @param    receiveTimeout_ms   Total time until the low level receive routine returns
* @param    writeTimeout_ms     Total time for writing a message
* @returns true when the timeouts config was successfull, false otherwise
************************************************************************************/
bool SerialMessageHandler::configureTimeouts(uint32_t receiveTimeout_ms, uint32_t writeTimeout_ms)
{
    return m_commInterface->configureReadTimeouts(RECEIVE_BYTE_TO_BYTE_TIMEOUT_MS, receiveTimeout_ms);
}

/*******************************************************************************//***
* \brief Open a serial connection to the device
*
* @param    portNo      Port number of the COM port to be opened.
* @param    baudrate    Baudrate that gets configured for the port.
* @returns true when connection has been established successfully, false otherwise
************************************************************************************/
bool SerialMessageHandler::establishConnection(uint8_t portNo, uint32_t baudrate)
{
    // TODO: Error handling?
    m_connectionEstablished = m_commInterface->openPort(portNo, baudrate);
    
    return m_connectionEstablished;
}
