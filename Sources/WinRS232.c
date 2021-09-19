/********************************************************************************//**
* \file WinRS232.c
* \author Roman Holderried
*
* \brief Low level serial communication functionality.
*
* <b> History </b>
*      - 2021-09-19 - File creation. 
*                     
***********************************************************************************/


#include "WinRS232.h"
#include <stdint.h>

/***********************************************************************************
* Function definitions
***********************************************************************************/

tRS232ERROR RS232Open(tWINRS232* pInst, uint8_t portNo, uint32_t baudrate, tBYTESIZE byteSize, tPARITY parity, tSTOPBITS stopbits)
{
  const char  *comports[RS232_MAXPORTNR] = COM_NAME_ARRAY;
  uint8_t     portIdx = portNo - 1;
  HANDLE      hComm = NULL;

  if((portNo + 1 >= RS232_MAXPORTNR))
  {
    return(WINRS232_CONF_PORT_ILLEGAL);
  }

  hComm = CreateFileA(comports[portNo],
                      GENERIC_READ|GENERIC_WRITE,
                      0,                          /* no share  */
                      NULL,                       /* no security */
                      OPEN_EXISTING,
                      0,                          /* no threads */
                      NULL);                      /* no templates */

  if(hComm==INVALID_HANDLE_VALUE)
    return(WINRS232_UNABLE_TO_OPEN_PORT);

  DCB portSettings;
  memset(&portSettings, 0, sizeof(portSettings));  /* clear the new struct  */

  // Configure the DCB structure
  portSettings.DCBlength = sizeof(portSettings);
  portSettings.ByteSize = (DWORD)byteSize;
  portSettings.BaudRate = (DWORD)baudrate;
  portSettings.fBinary = 1;
  portSettings.Parity = (DWORD)parity;
  portSettings.StopBits = (DWORD)stopbits;
  // Since there is currently no flow control, all the other settings are fine with 0


  if(!SetCommState(hComm, &portSettings))
  {
    CloseHandle(hComm);
    return(WINRS232_CONF_SETTINGS_NOT_APPLICABLE);
  }

  COMMTIMEOUTS Cptimeouts;

  Cptimeouts.ReadIntervalTimeout         = 0;
  Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
  Cptimeouts.ReadTotalTimeoutConstant    = 0;
  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant   = 0;

  SetCommTimeouts(hComm, &Cptimeouts);

  // Set the structure
  pInst->connectionState  = true;
  pInst->portHandle       = hComm;
  pInst->portNo           = portNo;

  return(WINRS232_SUCCESS);
}


tRS232ERROR RS232ReadByteFromPort(tWINRS232 *pInst, uint8_t *buf, uint32_t size)
{
  uint32_t bytesRead;

/* added the void pointer cast, otherwise gcc will complain about */
/* "warning: dereferencing type-punned pointer will break strict aliasing rules" */

  if(!ReadFile(pInst->portHandle, buf, size, (LPDWORD)((void *)&bytesRead), NULL))
  {
    return WINRS232_PORT_READOUT_FAILED;
  }

  return WINRS232_SUCCESS;
}


tRS232ERROR RS232SendByte(tWINRS232 *pInst, uint8_t byte)
{
  uint32_t bytesSent;

  if(!WriteFile(pInst->portHandle, &byte, 1, (LPDWORD)((void *)&bytesSent), NULL))
  {
    return(WINRS232_SEND_OPERATION_FAILED);
  }

  if(bytesSent<0)  
    return(WINRS232_SEND_OPERATION_FAILED);

  return WINRS232_SUCCESS;
}


tRS232ERROR RS232SendBuffer(tWINRS232 *pInst, uint8_t *buffer, uint32_t size)
{
  uint32_t bytesSent;

  if(!WriteFile(pInst->portHandle, buffer, size, (LPDWORD)((void *)&bytesSent), NULL))
  {
    return(WINRS232_SEND_OPERATION_FAILED);
  }

  return WINRS232_SUCCESS;
}


void RS232ClosePort(tWINRS232 *pInst)
{
  CloseHandle(pInst->portHandle);
}


// int RS232_IsDCDEnabled(int comport_number)
// {
//   int status;

//   GetCommModemStatus(Cport[comport_number], (LPDWORD)((void *)&status));

//   if(status&MS_RLSD_ON) return(1);
//   else return(0);
// }


// int RS232_IsRINGEnabled(int comport_number)
// {
//   int status;

//   GetCommModemStatus(Cport[comport_number], (LPDWORD)((void *)&status));

//   if(status&MS_RING_ON) return(1);
//   else return(0);
// }


// int RS232_IsCTSEnabled(int comport_number)
// {
//   int status;

//   GetCommModemStatus(Cport[comport_number], (LPDWORD)((void *)&status));

//   if(status&MS_CTS_ON) return(1);
//   else return(0);
// }


// int RS232_IsDSREnabled(int comport_number)
// {
//   int status;

//   GetCommModemStatus(Cport[comport_number], (LPDWORD)((void *)&status));

//   if(status&MS_DSR_ON) return(1);
//   else return(0);
// }


// void RS232_enableDTR(int comport_number)
// {
//   EscapeCommFunction(Cport[comport_number], SETDTR);
// }


// void RS232_disableDTR(int comport_number)
// {
//   EscapeCommFunction(Cport[comport_number], CLRDTR);
// }


// void RS232_enableRTS(int comport_number)
// {
//   EscapeCommFunction(Cport[comport_number], SETRTS);
// }


// void RS232_disableRTS(int comport_number)
// {
//   EscapeCommFunction(Cport[comport_number], CLRRTS);
//}

/*
https://msdn.microsoft.com/en-us/library/windows/desktop/aa363428%28v=vs.85%29.aspx
*/

void RS232FlushRX(tWINRS232* pInst)
{
  PurgeComm(pInst->portHandle, PURGE_RXCLEAR | PURGE_RXABORT);
}


void RS232FlushTX(tWINRS232* pInst)
{
  PurgeComm(pInst->portHandle, PURGE_TXCLEAR | PURGE_TXABORT);
}


void RS232FlushRXTX(tWINRS232* pInst)
{
  PurgeComm(pInst->portHandle, PURGE_RXCLEAR | PURGE_RXABORT);
  PurgeComm(pInst->portHandle, PURGE_TXCLEAR | PURGE_TXABORT);
}










