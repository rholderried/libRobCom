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
#define MSG_BUFFER_WIDTH            1024
/** Defines how many messages can be hold by the debug message buffer and the 
 * RobComSerial message buffer*/
#define MSG_BUFFER_SIZE             10

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
        
        void msgStateMachine (void);



    private:
        ThreadWrapper       *m_eventHandlerPtr      = nullptr;
        // Ringbuffer holding the debug messages
	    Ringbuffer<uint8_t> *m_liveDatalogBuffer    = nullptr;
        SerialInterface     *m_commInterface        = nullptr;
        uint8_t             m_msgBuf[MSG_BUFFER_SIZE] = {0};
        uint32_t            m_actualBufferIdx         = 0;

        void msgReceiver (uint8_t *buffer, uint32_t size);
        
        

};