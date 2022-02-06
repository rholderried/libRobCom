/********************************************************************************//**
 * \file SerialMessageHandler.h
 * \author Roman Holderried
 *
 * \brief RobCom serial interface.
 *
 * <b> History </b>
 *      - 2021-09-26 - File creation. 
 *                     
 ***********************************************************************************/

#ifndef SERIALMESSAGEHANDLER_H_
#define SERIALMESSAGEHANDLER_H_

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include <iostream>
#include <cstdint>
#include <cstdbool>
#include "Helpers.h"
#include "SerialInterface.h"
#include <mutex>

/************************************************************************************
 * Defines
 ***********************************************************************************/

/** Define the width of the receive buffer (Stack buffer!)*/
#define RECEIVE_BUFFER_WIDTH                    128
#define RECEIVE_BYTE_TO_BYTE_TIMEOUT_MS         10

/************************************************************************************
 * Type definitions
 ***********************************************************************************/
typedef std::function<void()> ActionProcedure;

struct callbacks {
    ActionProcedure onReceivedMsg;
    ActionProcedure onTransmitMsg;
};

/************************************************************************************
 * Class definitions
 ***********************************************************************************/
class SerialMessageHandler
{
    public:

        uint32_t                m_bufWidth;
        uint32_t                m_bufSize;
        uint8_t                 m_msgTermSymbol;
        bool                    m_terminateRec;
        bool                    m_receiveImmediately; 
        Ringbuffer<uint8_t>     *m_msgRingBuffer        = nullptr;

        struct callbacks        m_callbacks = {nullptr, nullptr};
        
        

        SerialMessageHandler(uint32_t bufWidth, uint32_t bufSize, uint8_t msgTermSymbol);
        ~SerialMessageHandler();
        
        //void msgStateMachine (void);
        bool establishConnection(uint8_t portNo, uint32_t baudrate);
        bool startReceiver(bool enableReceiveState = false);
        bool configureTimeouts(uint32_t receiveTimeout_ms, uint32_t writeTimeout_ms);
        bool setInterfaceToReceiveState(void);

    private:
        
        ThreadWrapper       *m_eventHandlerPtr      = nullptr;
        SerialInterface     *m_commInterface        = nullptr;
        // Ringbuffer holding the debug messages
	    
        uint8_t             *m_msgBuf;
        uint32_t            m_actualBufferIdx       = 0;
        bool                m_connectionEstablished = false;

        void msgReceiver (uint8_t *buffer, uint32_t size);

    protected:
        std::mutex          m_terminatorMutex;

};

#endif //SERIALMESSAGEHANDLER_H_