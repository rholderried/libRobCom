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
#include <mutex>
#include <chrono>
#include "Helpers.h"
#include "SerialMessageHandler.h"
#include "RobComSerial.h"
#include "CommonTypes.h"


/************************************************************************************
 * Class member definitions
 ***********************************************************************************/

/*******************************************************************************//***
* \brief RobComSerial c'tor
************************************************************************************/
RobComSerial::RobComSerial(uint32_t bufWidth, uint32_t bufSize, tMESSAGE *request, tMESSAGE *response):SerialMessageHandler(bufWidth, bufSize, ETX)
{
    m_RxErrByte = 0;
    m_TxErrByte = 0;
    m_receiveReady = false; 
    m_request = request;
    m_response = response;  
}

/*******************************************************************************//***
* \brief RobComSerial d'tor
************************************************************************************/
RobComSerial::~RobComSerial()
{
    // Delete the state machine thread if it is one running
    if (m_stateMachineRunning)
    {
        // if (m_state ==  )
        m_stateMachineRunning = false;
        delete m_stateMachineHandler;
    }
    // The base class destructor gets called automatically
}

/*******************************************************************************//***
* \brief Sets the receive ready flag.
************************************************************************************/
void RobComSerial::onReceivedMsg(void)
{
    std::lock_guard<std::mutex> lock(m_stateMachineMutex);
    // Wake up the state machine and tell it that we have received a message
    m_receiveReady = true;
    m_stateMachineControl.notify_all();
}

/*******************************************************************************//***
* \brief Initialization functionality of RobComSerial
************************************************************************************/
bool RobComSerial::RobComSerialInit(uint8_t portNo, uint32_t baudrate, uint32_t receiveTimeout_ms)
{
    bool successFlag = false;

    m_receiveTimeout_ms = std::chrono::milliseconds(receiveTimeout_ms);

    successFlag |= establishConnection(portNo, baudrate);

    if (successFlag &= configureTimeouts(RECEIVE_RETURN_TIMEOUT, 0))
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

/*******************************************************************************//***
* \brief RobComSerial state machine
************************************************************************************/
void RobComSerial::stateMachine(void)
{
    std::unique_lock<std::mutex> stateLock(m_stateMachineMutex);

    while(m_stateMachineRunning)
    {
        switch(m_state)
        {
            case ROBCOMSERIAL_IDLE:
                // While idle, the state machine shall sleep
                while(m_state == ROBCOMSERIAL_IDLE)
                    m_stateMachineControl.wait(stateLock);
                break;

            case ROBCOMSERIAL_RECEIVE:
                // Wake up the receiver thread
                startReceiver();

                // Go to bed again, wait for the timeout or the ready flag
                if (m_stateMachineControl.wait_for(stateLock, m_receiveTimeout_ms, [this]{return m_receiveReady;}))
                {
                    setState(ROBCOMSERIAL_EVALUATE);
                    // At this time m_receiveReady cannot be set by another thread, so no lock required.
                    m_receiveReady = false;
                }
                else
                {
                    // Acquire lock for the terminator variable. In the meanwhile, the last symbol can arrive.
                    std::lock_guard<std::mutex> teminatorLock(m_terminatorMutex);
                    // last chance: Did we receive something???
                    if (m_receiveReady)
                    {
                        setState(ROBCOMSERIAL_EVALUATE);
                        // At this time m_receiveReady cannot be set by another thread, so no lock required.
                        m_receiveReady = false;
                    }
                    else
                    {
                        m_terminateRec = true;
                        m_RxErrBits.bRxTimeout = 1;
                        // TODO: Error handling
                    }
                }
                break;

            case ROBCOMSERIAL_EVALUATE:
                {
                    uint8_t tempRec []
                }

                break;

            case ROBCOMSERIAL_TRANSMIT:
                break;
        }
    }
}

/*******************************************************************************//***
* \brief Thread-safe setter for the RobComSerial Statemachine
************************************************************************************/
void RobComSerial::setState(ROBCOMSERIAL_STATE newState)
{
    ROBCOMSERIAL_STATE oldState = m_state;
    
    {
        std::lock_guard<std::mutex> lock(m_stateMachineMutex);
        m_state = newState;
    }
    // Wake up the state machine (maybe not necessary, but anyways....)
    if (!(newState == ROBCOMSERIAL_IDLE || newState == ROBCOMSERIAL_RECEIVE))
        m_stateMachineControl.notify_all();
}

/*******************************************************************************//***
* \brief Evaluates the RobComSerial message
************************************************************************************/
void RobComSerial::evaluateMessage(void)
{

}

