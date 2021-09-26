/********************************************************************************//**
 * \file SerialInterface.c
 * \author Roman Holderried
 *
 * \brief SerialInterface method definitions. 
 * 
 * SerialInterface acts as the main communication handler for serial communication 
 * with RobCom embedded devices. It handles message routing for the RobComSerial
 * protocol and the debug message interface.
 * It also provides message buffer space.
 *
 * <b> History </b>
 *      - 2021-07-18 - File creation. 
 *                     
 ***********************************************************************************/

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include "SerialInterface.h"
#include "Helpers.h"

#ifdef OS_WIN32
#include "WinRS232.h"
#elif defined(OS_LINUX)
#endif


/*******************************************************************************//***
* \brief DefCon of the Serial interface.
*
* Initializes a SerialInterface instance
* 
* @param    receive_cb  Function that gets called when new data has arrived (or a
*                       comm timeout occured).
************************************************************************************/
SerialInterface::SerialInterface(ReceiveCallback receive_cb, uint32_t receiveSize)
{
    m_ifState               = SERIAL_INTERFACE_IDLE;
    m_receive_cb            = receive_cb;
    m_receiveSize           = receiveSize;
    m_receiveTaskRunning    = false;
}

/*******************************************************************************//***
* \brief Port opening constructor of the serial interface.
*
* Similar to DefCon, automatically opens a serial port.
*
* @param    receive_cb  Function that gets called when new data has arrived (or a
*                       comm timeout occured).
* @param    portNo      Port number of the COM port to be opened.
* @param    baudrate    Baudrate that gets configured for the port.
************************************************************************************/
SerialInterface::SerialInterface(ReceiveCallback receive_cb, uint32_t receiveSize, uint8_t portNo, uint32_t baudrate)
{
    m_ifState = SERIAL_INTERFACE_IDLE;
    m_receive_cb = receive_cb;
    m_receiveSize = receiveSize;

    this->openPort(portNo, baudrate);
}

/*******************************************************************************//***
* \brief Opens a serial port (RS232)
*
* Uses the standard 8N1 configuration for serial communication.
*
* @param    portNo      Port number of the COM port to be opened.
* @param    baudrate    Baudrate that gets configured for the port.
* @returns  Success indicator
************************************************************************************/
bool SerialInterface::openPort(uint8_t portNo, uint32_t baudrate)
{
    bool retVal;
    // Lower level RS232 opening function
    retVal = (RS232Open(&this->m_Rs232, portNo, baudrate, BYTE_SIZE_8, NO_PARITY, STOPBITS_1_0) == 0);

    return retVal;
}

/*******************************************************************************//***
* \brief Function that receives data from the serial port
*
* Usually runs in its own thread in order to not block the main application. Calls 
* the receive callback when data has arrived.
*
* @param    buffer              Pointer to the buffer the data gets stored in. 
* @param    size                Data size that is to be received.
* @param    intervalTimeout_ms  Maximum Byte-to-byte timespan of the data stream
*                               before the Readfile function returns.
************************************************************************************/
void SerialInterface::receiveTask(void)
{
    uint8_t receiveBuffer[m_receiveSize] = {0};
    int32_t actualReceived = 0;

    m_ifState = SERIAL_INTERFACE_RECEIVING;
    // Set timeout
    // RS232ConfigureReadTimeout(&m_Rs232, intervalTimeout_ms);

    while (m_receiveTaskRunning)
    {
        actualReceived = RS232ReadBufferFromPort(&m_Rs232, receiveBuffer, m_receiveSize);

        // Call the data processing receive routine
        m_receive_cb(receiveBuffer, actualReceived);
    }

    m_ifState = SERIAL_INTERFACE_IDLE;
}

/*******************************************************************************//***
* \brief Function that sends data via the serial port
*
* @param    buffer              Pointer to the buffer the data is stored in. 
* @param    size                Data byte size that gets sent.
************************************************************************************/
void SerialInterface::sendBuffer(uint8_t* buffer, uint32_t size)
{
    this->m_ifState = SERIAL_INTERFACE_SENDING;

    RS232SendBuffer(&this->m_Rs232, buffer, size);

    this->m_ifState = SERIAL_INTERFACE_IDLE;
}