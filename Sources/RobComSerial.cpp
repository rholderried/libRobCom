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

