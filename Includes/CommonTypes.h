/********************************************************************************//**
* \file CommonTypes.h
* \author Roman Holderried
*
* \brief Type declarations that have to be commonly known by the modules.
*
* <b> History </b>
*      - 2022-02-06 - File creation. 
*                     
***********************************************************************************/
#ifndef COMMONTYPES_H_
#define COMMONTYPES_H_

/***********************************************************************************
* Includes
***********************************************************************************/
#include <cstdint>
#include <cstdbool>

/***********************************************************************************
* Typedefs
***********************************************************************************/
typedef struct
{
    bool bPending;
    union
    {
        struct
        {
            uint16_t     uiSubModuleID  : 5;
            uint16_t     uiCommandType  : 3;
            uint16_t     uiCommandID    : 7;
            uint16_t     uiExtended     : 1;
        }sCommandFields;

        uint16_t    ui16CommandField;
    };
    uint16_t        ui16FrameLen;
    uint8_t         *pui8FrameBuf;
}tMESSAGE;

#define tMESSAGE_DEFAULTS {false, {.ui16CommandField = 0}, 0, nullptr}


#endif // COMMONTYPES_H_