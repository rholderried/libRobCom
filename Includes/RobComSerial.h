/********************************************************************************//**
 * \file RobComSerial.h
 * \author Roman Holderried
 *
 * \brief RobComSerial protocol types and definitions
 *
 * <b> History </b>
 *      - 2021-09-19 - File creation. 
 *                     
 ***********************************************************************************/

#ifndef ROBCOMSERIAL_H_
#define ROBCOMSERIAL_H_

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include <iostream>
#include <cstdint>
#include <cstdbool>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include "Helpers.h"
#include "SerialMessageHandler.h"
#include "SerialInterface.h"

/************************************************************************************
 * Defines
 ***********************************************************************************/

// STX, Start of Transmission
#ifndef STX
#define STX			0x02
#endif

// ETX, End of Transmission
#ifndef ETX
#define ETX			0x03
#endif

// DC1
#ifndef DC1 
#define DC1 		0x11
#endif

// Acknowledge
#ifndef ACK
#define ACK			0x06
#endif

// Not Acknowledge
#ifndef NAK
#define NAK			0x15
#endif

// Escape Byte
#ifndef ESC
#define ESC         0x1B
#endif

#define RECEIVE_RETURN_TIMEOUT 50  // ms until the serial device receiver routine returns

/************************************************************************************
 * Type definitions
 ***********************************************************************************/
typedef enum
{
    ROBCOMSERIAL_IDLE,
    ROBCOMSERIAL_TRANSMIT,
    ROBCOMSERIAL_WAITING_FOR_ACK,
    ROBCOMSERIAL_RECEIVE,
    ROBCOMSERIAL_EVALUATE,
    ROBCOMSERIAL_ERROR,
    ROBCOMSERIAL_ABORT
}ROBCOMSERIAL_STATE;

/************************************************************************************
 * Class definitions
 ***********************************************************************************/
class RobComSerial: public SerialMessageHandler
{
    public:
    std::chrono::milliseconds m_receiveTimeout_ms;
    tMESSAGE *m_request;
    tMESSAGE *m_response;

    RobComSerial(uint32_t bufWidth, uint32_t bufSize, tMESSAGE *request, tMESSAGE *response);
    ~RobComSerial();

    bool RobComSerialInit(uint8_t portNo, uint32_t baudrate, uint32_t receiveTimeout_ms);
    void onReceivedMsg(void);


    private:

    bool    m_receiveReady;
    
    void stateMachine(void);
    void setState(ROBCOMSERIAL_STATE newState);
    void evaluateMessage(void);

    // State machine control mutex
    std::condition_variable m_stateMachineControl;
    std::mutex m_stateMachineMutex;

    ROBCOMSERIAL_STATE  m_state;
    ThreadWrapper *m_stateMachineHandler = nullptr;
    bool m_stateMachineRunning = false;

    union {
        struct {
            uint8_t bRxForbiddenChar    : 1;
            uint8_t bRxExcessiveLength  : 1;
            uint8_t bRxWrongChecksum    : 1;
            uint8_t bRxTimeout          : 1;
            uint8_t bReserved3          : 1;
            uint8_t bReserved4          : 1;
            uint8_t bReserved5          : 1;
            uint8_t bReserved6          : 1;
        }m_RxErrBits;
        uint8_t m_RxErrByte;
    };

    union {
        struct {
            uint8_t bTxTimeout          : 1;
            uint8_t bReserved0          : 1;
            uint8_t bReserved1          : 1;
            uint8_t bReserved2          : 1;
            uint8_t bReserved3          : 1;
            uint8_t bReserved4          : 1;
            uint8_t bReserved5          : 1;
            uint8_t bReserved6          : 1;
        }m_TxErrBits;
        uint8_t m_TxErrByte;
    };

    void protocolDecode (void);
     
};
#endif // ROBCOMSERIAL_H_