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
#include <stdint.h>
#include "Helpers.h"
#include "Configuration.h"

#ifdef OS_WIN32
#include "WinRS232.h"
#elif defined(OS_LINUX)
#endif


class SerialInterface
{
public:
    tRS232 m_rs232;

    SerialInterface();
    SerialInterface(uint8_t portNo, uint32_t baudrate);
};



#endif \\ SERIALINTERFACE_H_