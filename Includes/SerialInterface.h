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
#include <cstdint>
#include <cstdbool>
#include <functional>
#include <condition_variable>
#include <mutex>
#include "Configuration.h"

#ifdef OS_WIN32
#include "WinRS232.h"
#elif defined(OS_LINUX)
#endif


/************************************************************************************
 * Defines
 ***********************************************************************************/

// #define BYTE_TO_BYTE_TIMEOUT_MS     10

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

typedef std::function<void(uint8_t*, uint32_t)> ReceiveCallback;

/************************************************************************************
 * Class definitions
 ***********************************************************************************/
class SerialInterface
{
public:
    
    tSERIALIFSTATE          m_ifState;
    ReceiveCallback         m_receive_cb;
    uint32_t                m_receiveSize;
    bool                    m_receiveTaskRunning;
    bool                    m_readyToReceive;
    std::condition_variable m_cond;
    std::mutex              m_mtx;


    SerialInterface(ReceiveCallback receive_cb, uint32_t receiveSize);
    SerialInterface(ReceiveCallback receive_cb, uint32_t receiveSize, uint8_t portNo, uint32_t baudrate);
    bool openPort(uint8_t portNo, uint32_t baudrate);
    bool configureReadTimeouts(uint32_t byteToByteTimeout_ms = 0, uint32_t totalTimeoutConstant_ms = 0, uint32_t totalTimeoutMultiplier = 0);
    // void changeReceiveTimeout(uint32_t timeout_ms);
    void receiveTask(void);
    void sendBuffer(uint8_t* buffer, uint32_t size);

private:
    tRS232 m_Rs232;
};

#endif // SERIALINTERFACE_H_