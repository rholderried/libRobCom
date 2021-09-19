/********************************************************************************//**
 * \file SerialInterface.c
 * \author Roman Holderried
 *
 * \brief SerialInterface method definitions. 
 * 
 * SerialInterface acts as the main communication handler for serial communication 
 * with RobCom embedded devices. It handles message routing for the RobComSerial
 * protocol and the debug message interface.
 *
 * <b> History </b>
 *      - 2021-07-18 - File creation. 
 *                     
 ***********************************************************************************/

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include "SerialInterface.h"

/***************************************************************************//**
* \brief DefCon of the Serial interface.
*
* Initializes a SerialInterface instance
*******************************************************************************/
SerialInterface::SerialInterface()
{

}