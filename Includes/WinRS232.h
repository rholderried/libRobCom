/********************************************************************************//**
* \file WinRS232.h
* \author Roman Holderried
*
* \brief Typedefs, function prototypes etc. for the Windows RS232 interface.
*
* <b> History </b>
*      - 2021-09-19 - File creation. 
*                     
***********************************************************************************/

/***********************************************************************************
* Defines
***********************************************************************************/

#ifndef WINRS232_H_
#define WINRS232_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <windows.h>
#define RS232_MAXPORTNR  32

#define COM_NAME_ARRAY     {"\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",  "\\\\.\\COM4",  \
                            "\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",  "\\\\.\\COM8",  \
                            "\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11", "\\\\.\\COM12", \
                            "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15", "\\\\.\\COM16", \
                            "\\\\.\\COM17", "\\\\.\\COM18", "\\\\.\\COM19", "\\\\.\\COM20", \
                            "\\\\.\\COM21", "\\\\.\\COM22", "\\\\.\\COM23", "\\\\.\\COM24", \
                            "\\\\.\\COM25", "\\\\.\\COM26", "\\\\.\\COM27", "\\\\.\\COM28", \
                            "\\\\.\\COM29", "\\\\.\\COM30", "\\\\.\\COM31", "\\\\.\\COM32"}  

/***********************************************************************************
* Type definitions
***********************************************************************************/
typedef struct 
{
    bool        connectionState;
    uint8_t     portNo;
    HANDLE      portHandle;
}tWINRS232;

typedef enum
{
    NO_PARITY,
    ODD_PARITY,
    EVEN_PARITY,
    MARK_PARITY,
    SPACE_PARITY
}tPARITY;

typedef enum
{
    WINRS232_SUCCESS,
    WINRS232_CONF_PORT_ILLEGAL = 1,
    WINRS232_CONF_INVALID_BYTE_SIZE = 2,
    WINRS232_CONF_SETTINGS_NOT_APPLICABLE = 3,

    WINRS232_UNABLE_TO_OPEN_PORT = 10,
    WINRS232_PORT_READOUT_FAILED = 11,
    WINRS232_SEND_OPERATION_FAILED = 12
}tRS232ERROR;

typedef enum
{
    STOPBITS_1_0 = 0,
    STOPBITS_1_5 = 1,
    STOPBITS_2_0 = 2
}tSTOPBITS;

typedef enum
{
    BYTE_SIZE_5 = 5,
    BYTE_SIZE_6 = 6,
    BYTE_SIZE_7 = 7,
    BYTE_SIZE_8 = 8
}tBYTESIZE;

tRS232ERROR RS232Open(tWINRS232* pInst, uint8_t portNo, uint32_t baudrate, tBYTESIZE byteSize, tPARITY parity, tSTOPBITS stopbits);
tRS232ERROR RS232ReadByteFromPort(tWINRS232 *pInst, uint8_t *buf, uint32_t size);
tRS232ERROR RS232SendByte(tWINRS232 *pInst, uint8_t byte);
tRS232ERROR RS232SendBuffer(tWINRS232 *pInst, uint8_t *buffer, uint32_t size);
void RS232ClosePort(tWINRS232 *pInst);
// void RS232_cputs(int, const char *);
// int RS232_IsDCDEnabled(int);
// int RS232_IsRINGEnabled(int);
// int RS232_IsCTSEnabled(int);
// int RS232_IsDSREnabled(int);
// void RS232_enableDTR(int);
// void RS232_disableDTR(int);
// void RS232_enableRTS(int);
// void RS232_disableRTS(int);
void RS232FlushRX(tWINRS232* pInst);
void RS232FlushTX(tWINRS232* pInst);
void RS232FlushRXTX(tWINRS232* pInst);
// int RS232_GetPortnr(const char *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // WINRS232_H_


