/*! brief A test class

 * RoProtocol.h
 *
 *  Created on: 19.08.2019
 *      Author: roman
 */

#ifndef ROBCOMUSB_H_
#define ROBCOMUSB_H_

#include <stdint.h>
#include <iostream>
#include <mutex>
#include "USBInterface.h"
#include "Helpers.h"
#include "Variables.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define CONTROL_TRANSFER_BUFFER_SZ 73
#define CONTROL_TRANSFER_MAX_DATA_SZ 64
#define CONTROL_TRANSFER_TIMEOUT_MS 100
// Control Transfer bmRequestType definitions
#define CONTROL_TRANSFER_BIDIRECTIONAL 0xC0
#define CONTROL_TRANSFER_UNIDIRECTIONAL 0x40
// bRequest field defining a command request via EP0
#define GET_COMMAND_BREQUEST 0x01
#define SET_COMMAND_BREQUEST 0x02
// Bulk transfer settings
#define DATA_IN_EP_ADDRESS 0x81
#define MEMORY_READOUT_BUFFER_SZ 8096
#define MEMORY_READOUT_TIMEOUT_MS 200
#define LIVE_DATALOG_BUFFER_WIDTH 64
#define LIVE_DATALOG_BUFFER_SIZE 64

/*******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef enum {
	GET_NONE 				= 0x00,
	GET_EEPROM_VARIABLE		= 0x01,
	GET_RAM_VARIABLE		= 0x02,
	GET_DEVICE_DESCRIPTION	= 0x03,
	GET_EEPROM_VAR_DESC		= 0x04,
	GET_RAM_VAR_DESC		= 0x05
}DEFAULT_GET_CMD_ID;

typedef enum {
	SET_NONE						= 0x00,
	SET_EEPROM_VARIABLE				= 0x01,
	SET_RAM_VARIABLE				= 0x02,
	// Datalog IDs
	DATALOG_INTERNAL_INIT			= 0xA0,
	DATALOG_INTERNAL_START          = 0xA1,
	DATALOG_INTERNAL_STOP           = 0xA2,
	DATALOG_INTERNAL_SET_FROM_MEMORY= 0xA3,
	DATALOG_LIVE_INIT              	= 0xA4,
	DATALOG_LIVE_START              = 0xA5,
	DATALOG_LIVE_STOP               = 0xA6,
	DATALOG_READ_MEMORY             = 0xA7
}DEFAULT_SET_CMD_ID;

typedef enum {
	GET_COMMAND_IDX 								= 0,
	SET_COMMAND_IDX 								= 1,
	// Datalog transfers
	DATALOG_INTERNAL_INIT_COMMAND_IDX 				= 2,
	DATALOG_INTERNAL_START_COMMAND_IDX 				= 3,
	DATALOG_INTERNAL_STOP_COMMAND_IDX 				= 4,
	DATALOG_INTERNAL_SET_FROM_MEMORY_COMMAND_IDX 	= 5,
	DATALOG_LIVE_INIT_COMMAND_IDX 					= 6,
	DATALOG_LIVE_START_COMMAND_IDX 					= 7,
	DATALOG_LIVE_STOP_COMMAND_IDX 					= 8,
	DATALOG_MEMORY_READOUT_COMMAND_IDX 				= 9,
	DATALOG_MEMORY_READOUT_DATA_IDX 				= 10,
	DATALOG_LIVE_DATA_IDX							= 11
}TRANSFERIDX;


// All USB error definitions
typedef enum
{
	/** No error occured **/
	ERROR_NONE = 0,
	// Errors defined by libusb
	/** Input/output error */
	USB_ERROR_IO = -1,
	/** Invalid parameter */
	USB_ERROR_INVALID_PARAM = -2,
	/** Access denied (insufficient permissions) */
	USB_ERROR_ACCESS = -3,
	/** No such device (it may have been disconnected) */
	USB_ERROR_NO_DEVICE = -4,
	/** Entity not found */
	USB_ERROR_NOT_FOUND = -5,
	/** Resource busy */
	USB_ERROR_BUSY = -6,
	/** Operation timed out */
	USB_ERROR_TIMEOUT = -7,
	/** Overflow */
	USB_ERROR_OVERFLOW = -8,
	/** Pipe error */
	USB_ERROR_PIPE = -9,
	/** System call interrupted (perhaps due to signal) */
	USB_ERROR_INTERRUPTED = -10,
	/** Insufficient memory */
	USB_ERROR_NO_MEM = -11,
	/** Operation not supported or unimplemented on this platform */
	USB_ERROR_NOT_SUPPORTED = -12,
	// Errors defined by RobCom
	/** An USBDEVICE has already beed connected **/
	USB_ERROR_DEVICE_ALREADY_CONFIGURED = -13,
	/** Data Submission error: No Data **/
	USB_ERROR_SUBMISSION_NO_DATA= -14,
	/** Data Submission error: Too much data **/
	USB_ERROR_SUBMISSION_TOO_MUCH_DATA = -15,

	/** Other Errors**/
	USB_ERROR_OTHERS = -99
}ROBCOMUSBERROR;

typedef enum
{
	/** Transfer successful completed **/
	USB_TRANSFER_COMPLETED,
	/** Transfer failed */
	USB_TRANSFER_ERROR,
	/** Transfer timed out */
	USB_TRANSFER_TIMED_OUT,
	/** Transfer was cancelled */
	USB_TRANSFER_CANCELLED,
	/** For bulk/interrupt endpoints: halt condition detected (endpoint
	 * stalled). For control endpoints: control request not supported. */
	USB_TRANSFER_STALL,
	/** Device was disconnected */
	USB_TRANSFER_NO_DEVICE,
	/** Device sent more data than requested */
	USB_TRANSFER_OVERFLOW,
}ROBCOMUSBTRANSFERSTATUS;

typedef enum
{
	LIVE_DATALOG_CONFIGURATION_ERROR
}DATALOGSTATUS;

typedef struct
{
	uint16_t 	pid;
	uint16_t 	vid;
	int	 		interface;
}COMCONFIG;

#define COMCONFIG_DEFAULTS {0, 0, 0}

typedef enum
{
	IDLE,
	START_REQUEST,
	RUNNING,
	STOP_REQUEST
}LIVEDATALOGSTATE;

typedef enum
{
	CONNECTED		=	0,
	DISCONNECTED	=	1
}CONNECTIONSTATE;

// Callback type called as a Reaction on Datalog Commands
typedef std::function<void(int)> 			StatusCallback;
typedef std::function<void(uint8_t*,int)>	DataCallback;

/*******************************************************************************
 * Class declarations
 ******************************************************************************/
class RobComUSB
{
public:
	// Pointer to the USB Interface
	USBInterface* m_ifPtr = nullptr;
	// Helper functions
	HelperFunctions* m_helPtr = nullptr;
	// Ringbuffer to store live datalog messages
	Ringbuffer<uint8_t>* m_liveDatalogBuffer = nullptr;
	// Live Datalog members
	LIVEDATALOGSTATE m_liveDatalogState = IDLE;
	CONNECTIONSTATE m_USBConnectionState = DISCONNECTED;

	// Callbacks
	StatusCallback 		m_datalogExceptionCB 	= nullptr;
	StatusCallback		m_OnConnectionCB 		= nullptr;
	StatusCallback 		m_OnDisconnectionCB 	= nullptr;
	StatusCallback		m_OnVarInitializeCB 	= nullptr;
	CommandCallback 	m_SetCmdCB 				= nullptr;
	CommandCallback 	m_GetCmdCB 				= nullptr;
	DataCallback		m_DeviceSpecCB 			= nullptr;

	// Public member functions
	RobComUSB(COMCONFIG comConfig);
	//RobComUSB(COMCONFIG comConfig, std::function<void(NEWCONNECTIONSTATE)> onConnectionStateChangeCB);
	//RobComUSB(COMCONFIG comConfig,std::function<void(int)>(onConnectionCB),std::function<void(int)>(onDisconnectionCB));
	int ConnectDevice(void);
	int RegisterGETCommand(CommandCallback cbFunction);
	int RegisterSETCommand(CommandCallback cbFunction);
	void RegisterBidirectionalCommand(uint8_t* bufPtr, uint8_t requestID, uint8_t commandIdx, CommandCallback cbFunction);
	void RegisterUnidirectionalCommand(uint8_t* bufPtr, uint8_t requestID, uint8_t commandIdx, CommandCallback cbFunction);
	void RegisterMemoryReadout(void);
	void RegisterDatalogTransfers(StatusCallback datalogExceptionCB);
	int GetDeviceSpecification(void);
	int InitVars(RAMVariables* RAMVarPtr, EEPROMVariables* EEPROMVarPtr);
	int StartLiveDatalog (uint8_t varCount, uint16_t* varArrayPtr, uint16_t sampleTimeMS);
	int StopLiveDatalog (void);
	int SubmitGETCommand(uint16_t commandID, uint16_t value);
	int SubmitMemoryReadoutCommand(uint32_t memoryOffset, uint32_t lastAddress);

	// Destructor
	~RobComUSB();

	//int SubmitGETCommand(uint8_t* bufPtr, uint8_t dataSize, uint16_t commandID);
	// From here: Templates for the control transfer submit commands
	template<typename ... argtype>
	int SubmitSETCommand (uint16_t commandID, uint16_t value, argtype ... args)
	{
		int errorCode = 0;
		uint8_t buffer[64];
		std::size_t dataSize;

		// Check the connection state
		if (m_USBConnectionState != CONNECTED)
			errorCode = 1; // TODO: Define errors!
		else
		{
			// Fill the buffer
			dataSize = m_helPtr->WriteUI8Buffer(buffer, args...);

			// Call the lower-level submit function
			errorCode = SubmitCommand(&buffer[0], static_cast<uint16_t>(dataSize), commandID, value, static_cast<uint8_t>(SET_COMMAND_IDX));
		}

		return errorCode;
	}

private:
	// Device description
	COMCONFIG m_comConfig = COMCONFIG_DEFAULTS;

	// Send and receive Buffer
	uint8_t m_getCommandBuffer[CONTROL_TRANSFER_BUFFER_SZ] = {0};
	uint8_t m_setCommandBuffer[CONTROL_TRANSFER_BUFFER_SZ] = {0};
	uint8_t m_startMemoryReadoutCommandBuffer[CONTROL_TRANSFER_BUFFER_SZ] = {0};
	uint8_t m_memoryReadoutBuffer[MEMORY_READOUT_BUFFER_SZ] = {0};

	// Private Members for the Live Datalogger
	uint16_t m_liveDatalogSampleTimeMS = 0;

	// Private Members for the variable structure
	// Pointers on the variable structures
	EEPROMVariables* 	m_EEPROMVariables		= nullptr;
	RAMVariables*		m_RAMVariables			= nullptr;
	uint16_t m_RAMVarCount = 0;
	uint16_t m_EEPROMVarCount = 0;

	// Mutexes
	std::mutex m_submitMutex;

	// Timers
	Timeout *m_liveDatalogTimeoutPtr;

	// Private member functions
	int SubmitCommand(uint8_t* bufPtr, uint16_t dataSize, uint16_t commandID, uint16_t value, uint8_t transferIdx);
	void DatalogCommandCB(void* pSetup, unsigned char* buf_ptr, int status);
	void MemoryReadoutLowLevelCB(void* pSetup, unsigned char* buf_ptr, int status);
	void LiveDatalogLowLevelCB(void* pSetup, unsigned char* buf_ptr, int status);
	void SetCommandCB(void* pSetup, uint8_t* bufPtr, int status);
	void GetCommandCB(void* pSetup, uint8_t* bufPtr, int status);
	void LiveDatalogSendDataRequest(void);
	void OnConnectionStateChange(NEWCONNECTIONSTATE newConnectionState);
};

#endif /* ROBCOMUSB_H_ */
