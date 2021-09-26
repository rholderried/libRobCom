/********************************************************************************//**
 * \file DebugMessages.h
 * \author Roman Holderried
 *
 * \brief RobCom serial interface.
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
#include "Helpers.h"
#include "SerialInterface.h"

/************************************************************************************
 * Defines
 ***********************************************************************************/

/** Define which message length can be get from each serial interface*/
#define MSG_BUFFER_WIDTH                    1024
/** Define the width of the receive buffer (Stack buffer!)*/
#define RECEIVE_BUFFER_WIDTH                128
/** Defines how many messages can be hold by the debug message buffer and the 
 * RobComSerial message buffer*/
#define MSG_BUFFER_SIZE                     10
#define DEBUG_MSG_BYTE_TO_BYTE_TIMEOUT_MS   10

/************************************************************************************
 * Type definitions
 ***********************************************************************************/


/************************************************************************************
 * Class definitions
 ***********************************************************************************/
class DebugMessages
{
    public:
        DebugMessages();
        ~DebugMessages();
        
        //void msgStateMachine (void);
        void establishConnection(uint8_t portNo, uint32_t baudrate);



    private:
        ThreadWrapper       *m_eventHandlerPtr      = nullptr;
        // Ringbuffer holding the debug messages
	    Ringbuffer<uint8_t> *m_msgRingBuffer        = nullptr;
        SerialInterface     *m_commInterface        = nullptr;
        uint8_t             *m_msgBuf;
        uint32_t            m_actualBufferIdx       = 0;
        ThreadWrapper       *m_eventHandlerPtr      = nullptr;
        bool                m_connectionEstablished = false;

        void msgReceiver (uint8_t *buffer, uint32_t size);
        
        

};