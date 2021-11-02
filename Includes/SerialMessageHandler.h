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

/************************************************************************************
 * Defines
 ***********************************************************************************/

/** Define the width of the receive buffer (Stack buffer!)*/
#define RECEIVE_BUFFER_WIDTH                    128

/************************************************************************************
 * Type definitions
 ***********************************************************************************/


/************************************************************************************
 * Class definitions
 ***********************************************************************************/
class SerialMessageHandler
{
    public:

        uint32_t    m_bufWidth;
        uint32_t    m_bufSize;
        uint8_t     m_msgTermSymbol;
        Ringbuffer<uint8_t> *m_msgRingBuffer        = nullptr;

        SerialMessageHandler(uint32_t bufWidth, uint32_t bufSize, uint8_t msgTermSymbol);
        ~SerialMessageHandler();
        
        //void msgStateMachine (void);
        bool establishConnection(uint8_t portNo, uint32_t baudrate);

    private:
        ThreadWrapper       *m_eventHandlerPtr      = nullptr;
        // Ringbuffer holding the debug messages
	    
        SerialInterface     *m_commInterface        = nullptr;
        uint8_t             *m_msgBuf;
        uint32_t            m_actualBufferIdx       = 0;
        bool                m_connectionEstablished = false;

        void msgReceiver (uint8_t *buffer, uint32_t size);

};

#endif //SERIALMESSAGEHANDLER_H_