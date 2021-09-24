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
#include <functional>
#include "Helpers.h"
#include "Configuration.h"


/************************************************************************************
 * Defines
 ***********************************************************************************/
/** Define which message length can be get from each serial interface*/
#define MSG_BUFFER_WIDTH    4096
/** Defines how many messages can be hold by the debug message buffer and the 
 * RobComSerial message buffer*/
#define MSG_BUFFER_SIZE     10
#define MAX_NUM_PORTS       10

#define COM_TYPE_DEBUG      (uint8_t)1
#define COM_TYPE_DATA       (uint8_t)(1 << 1)
/************************************************************************************
 * Type definitions
 ***********************************************************************************/
typedef enum
{
    SERIAL_INTERFACE_IDLE,
    SERIAL_INTERFACE_RECEIVING,
    SERIAL_INTERFACE_SENDING
}tSERIALIFSTATE;

typedef std::function<bool(uint8_t*, uint32_t)> ReceiveCallback;

class SerialInterface
{
public:
    
    tSERIALIFSTATE m_ifState;
    ReceiveCallback m_receive_cb;

    SerialInterface(ReceiveCallback receive_cb);
    SerialInterface(ReceiveCallback receive_cb, uint8_t portNo, uint32_t baudrate);
    void openPort(uint8_t portNo, uint32_t baudrate);
    // void changeReceiveTimeout(uint32_t timeout_ms);
    void receiveTask(uint8_t* buffer, uint32_t size, uint32_t timeout_ms);

private:
    tRS232 m_Rs232;
};



#endif \\ SERIALINTERFACE_H_