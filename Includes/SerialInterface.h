/********************************************************************************//**
 * \file SerialInterface.h
 * \author Roman Holderried
 *
 * \brief RobCom serial interface.
 *
 * <b> History </b>
 *      - 2021-07-18 - File creation. 
 *                     
 ***********************************************************************************/

#ifndef SERIALINTERFACE_H_
#define SERIALINTERFACE_H_

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include <iostream>
#include <thread>
#include <cstdint>
#include <cstdbool>
#include "Helpers.h"
#include "Configuration.h"


#ifdef OS_WIN32
#include "WinRS232.h"
#elif defined(OS_LINUX)
#endif
/************************************************************************************
 * Defines
 ***********************************************************************************/
/** Define which message length can be get from each serial interface*/
#define MSG_BUFFER_WIDTH    4096
/** Defines how many messages can be hold by the debug message buffer and the 
 * RobComSerial message buffer*/
#define MSG_BUFFER_SIZE     10
#define MAX_NUM_PORTS       10
/************************************************************************************
 * Type definitions
 ***********************************************************************************/
typedef enum
{
    SERIAL_INTERFACE_IDLE,
    SERIAL_INTERFACE_RECEIVING,
    SERIAL_INTERFACE_SENDING
}tSERIALIFSTATE;

class SerialInterface
{
public:
    
    tSERIALIFSTATE m_actualIfState;
    tSERIALIFSTATE m_lastIfState;

    SerialInterface();
    SerialInterface(uint8_t portNo, uint32_t baudrate);
    bool openPort(uint8_t portNo, uint32_t baudrate);
    void handleSerialTransfers(void);

private:
    tRS232 m_rs232;
};



#endif \\ SERIALINTERFACE_H_