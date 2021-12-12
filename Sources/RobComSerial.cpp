/********************************************************************************//**
 * \file RobComSerial.cpp
 * \author Roman Holderried
 *
 * \brief RobComSerial protocol functionality
 *
 * <b> History </b>
 *      - 2021-11-01 - File creation. 
 *                     
 ***********************************************************************************/

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include <iostream>
#include <cstdint>
#include <cstdbool>
#include "Helpers.h"
#include "SerialMessageHandler.h"
#include "RobComSerial.h"


/************************************************************************************
 * Class member definitions
 ***********************************************************************************/

/*******************************************************************************//***
* \brief RobComSerial c'tor
************************************************************************************/
RobComSerial::RobComSerial(uint32_t bufWidth, uint32_t bufSize):SerialMessageHandler(bufWidth, bufSize, ETX)
{
    m_RxErrByte = 0;
    m_TxErrByte = 0;
}

/*******************************************************************************//***
* \brief RobComSerial d'tor
************************************************************************************/
RobComSerial::~RobComSerial()
{
    //delete 
}

void onReceivedMsg(void)
{
    ;
}

/*******************************************************************************//***
* \brief Initialization functionality of RobComSerial
************************************************************************************/
bool RobComSerial::RobComSerialInit(uint8_t portNo, uint32_t baudrate, uint32_t receiveTimeout_ms)
{
    bool successFlag = false;

    successFlag |= establishConnection(portNo, baudrate);

    if (successFlag &= configureTimeouts(receiveTimeout_ms, 0))
    {
        m_receiveImmediately = false;
        successFlag &= startReceiver(false);
    }

    // If the interface config succeeded, proceed with RobComSerial internal stuff
    if (successFlag)
    {
        m_state = ROBCOMSERIAL_IDLE;
        m_callbacks.onReceivedMsg = std::bind(onReceivedMsg, this); 
        // Start the RobComSerial protocol state machine
        m_stateMachineRunning = true;
        m_stateMachineHandler = new ThreadWrapper(std::bind(stateMachine, this));
    }
    return successFlag;
}



