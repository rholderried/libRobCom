/********************************************************************************//**
 * \file RobComShared.c
 * \author Roman Holderried
 *
 * \brief Exported RobCom functions. 
 *
 * <b> History </b>
 *      - 2021-09-30 - File creation. 
 *                     
 ***********************************************************************************/
/***********************************************************************************
* Includes
***********************************************************************************/
#include "RobCom.h"
#include <cstdint>
#include <iostream>

/***********************************************************************************
* Defines
***********************************************************************************/
#ifdef RELEASEBUILD
#define DLLEXPORT extern "C" __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/*******************************************************************************//***
* \brief Returns an instance pointer of the RobCom interface.
************************************************************************************/
DLLEXPORT RobCom* RobComInit()
{
    return new RobCom();
}

/*******************************************************************************//***
* \brief Initializes a serial connection
************************************************************************************/
DLLEXPORT void debugMsgInterface(RobCom* pInst, unsigned int portNo, unsigned int baudrate)
{
    bool success = pInst->establishSerialConnection(COMTYPE_DEBUGMESSAGES, portNo, baudrate);

    if (!success)
        throw "Connection to device could not be established.";
}

// /*******************************************************************************//***
// * \brief Writes the messages into the buffer passed by the application
// ************************************************************************************/
// DLLEXPORT uint8_t* getDebugMsg(RobCom* pInst)
// {
//     uint8_t * returnBuf;

//     if (pInst->getDebugMsg(pInst->m_debugMessages, &returnBuf) == 0)
//         return nullptr;
//     else
//         return returnBuf;
// }

/*******************************************************************************//***
* \brief Writes the messages into the buffer passed by the application
************************************************************************************/
DLLEXPORT uint32_t getDebugMsg(RobCom* pInst, uint8_t* buffer)
{
    return pInst->getDebugMsg(buffer);
}

/*******************************************************************************//***
* \brief Destroys the RobCom object.
************************************************************************************/
DLLEXPORT void deleteRobCom(RobCom* pInst)
{
    delete pInst;
}