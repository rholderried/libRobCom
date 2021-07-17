/*******************************************************************************
 * USBInterface.h
 *
 * Class definitions, structs and typedefs.
 *******************************************************************************
 * Date of creation: 19.08.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 ******************************************************************************/

#ifndef USBINTERFACE_H_
#define USBINTERFACE_H_

#include <iostream>
#include <functional>
#include "libusb.h"
#include <stdio.h>
#include <thread>
#include "Helpers.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define MAX_NUM_TRANSFERS 32

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

// Setupvariable f�r einen Control-Transfer
typedef struct __attribute__((packed))
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
}CONTROL_SETUP;

// Typdefinition for a callback
typedef std::function<void(void*, uint8_t*, int)> CommandCallback;

typedef enum
{
	CONTROL			= 0,
	BULK_IN			= 1,
	BULK_OUT		= 2,
	INTERRUPT_IN	= 3,
	INTERRUPT_OUT	= 4,
	ISO_IN			= 5,
	ISO_OUT			= 6
}TRANSFERTYPE;

typedef enum{
	DEVICE_CONNECTED				=	0,
	DEVICE_DISCONNECTED 			=	1
}NEWCONNECTIONSTATE;

// Transfer type
typedef struct
{
	libusb_transfer * transfer;
	CommandCallback transferCB;
	TRANSFERTYPE transferType;
	void* objPtr;
	uint8_t transferIdx;
}TRANSFER;

/***************************************************************************//**
 * Software modules that are representing the interface between the higher level
 * communication functions and the low level libusb library functions.
 ******************************************************************************/
class USBInterface
{
public:
	// Member variables
	bool m_hotplugCapability;
	bool m_libusbInitialized = false;
	uint32_t m_transfer_array_indizes = 0;
	uint32_t m_transfer_running = 0;
	std::function<void(NEWCONNECTIONSTATE)> m_onConnectionCB = nullptr; // This function is called in response to a connection attempt

	// Member functions
	USBInterface();
	int InitializeFunctionality(uint16_t pid, uint16_t vid, int interface);
	int DeinitializeFunctionality(void);
	void RegisterCtrlTransfer (uint8_t * bufferPtr, uint8_t bmRequestType, uint8_t bRequest,
	                           uint16_t timeout, uint8_t bufferSize, uint8_t transferIdx,
							   CommandCallback transferCB);
	void RegisterBulkTransfer (uint8_t * bufferPtr, uint8_t endpointAddress, uint16_t timeout,
							   uint32_t bufferSize, uint8_t transferIdx, CommandCallback transferCB);
	void EditCtrlTransfer(uint8_t* bufPtr, uint16_t wIndex, uint16_t wValue, uint16_t wLength, uint8_t transferIdx);
	int SubmitTransfer(uint8_t transferIdx);
	int SubmitTransfer(uint8_t transferIdx, uint8_t* bufferPtr);
	void DeregisterAllTransfers (void);
	void StopAllOutstandingTransfers (void);
	static void TransferCallback(libusb_transfer * transfer);
	~USBInterface();

private:
	// Private members
	libusb_device_handle * m_devh;
	bool m_eventHandlerRuns = false;
	ThreadWrapper *m_eventHandlerPtr = nullptr;
	TRANSFER m_transfer_array[MAX_NUM_TRANSFERS];


	// Private functions
	void EventHandlerThread (void);
};

#endif /* USBINTERFACE_H_ */
