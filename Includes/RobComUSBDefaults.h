/*******************************************************************************
 * RoBoardCom.h
 *
 * Application specific Code.
 *
 *******************************************************************************
 * Date of creation: 31.10.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 *******************************************************************************
 * TODO-List:
 ******************************************************************************/

#ifndef ROBCOMUSBDEFAULTS_H_
#define ROBCOMUSBDEFAULTS_H_

/******************************************************************************
* Includes
******************************************************************************/
#include "USBInterface.h"
#include "RobComUSB.h"
#include "Helpers.h"
#include <functional>

/******************************************************************************
* Defines
******************************************************************************/
#define LIVE_DATALOG_DATA_TIMEOUT_MS	20
// Command Ids
#define EEPROM_VAR_COMMAND_ID			0x01
#define RAM_VAR_COMMAND_ID				0x02


/******************************************************************************
* Typedefs
******************************************************************************/
typedef std::function<void(int)> USBComStatusCB;
typedef std::function<void(void*, unsigned char*, int)> USBComCommandCB;
typedef std::function<void(unsigned char*, int)> USBComDataCB;
//typedef std::function<void(DEVICESPEC)> USBComDeviceSpecCB;

// Configuration struct for the initialization
typedef struct
{
	int pid;
	int vid;
	int interface;
	USBComStatusCB 		OnConnectionCB;
	USBComStatusCB 		OnDisconnectionCB;
	USBComStatusCB 		DatalogExceptionCB;
	USBComStatusCB		OnVariablesInitCB;
	USBComCommandCB 	SetCommandCB;
	USBComCommandCB 	GetCommandCB;
	USBComDataCB 		LiveDatalogCB;
	USBComDataCB		DeviceSpecificationCB;
}CONFIG_USB;

#define CONFIG_USB_DEFAULTS {0, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}

typedef struct
{
	uint8_t 	protocolVersion;
	uint8_t 	softwareVersion;
	uint8_t 	softwareRevision;
	uint8_t 	hardwareVersion;
	uint8_t 	extHardwareVersion;
	uint16_t 	numberOfRAMVars;
	uint16_t	numberOfEEPROMVars;
}DEVICESPEC;

typedef struct __attribute__((packed))
{
	unsigned char requestType;
	unsigned char requestId;
	unsigned short commandValue;
	unsigned short commandId;
	unsigned short commandLength;
}COMMAND_SETUP;

enum LIVEDATALOGTIMERSTATE
{
	TIMER_RUNNING,
	TIMER_STOP_REQUEST,
	TIMER_STOPPED
};

class RobComUSBDefaults
{
public:
	// Public member variables
	RobComUSB *m_RobComUSB;
	enum LIVEDATALOGTIMERSTATE m_liveDatalogTimerState;
	DEVICESPEC m_deviceSpec;

	// This Callbacks are used for the interaction with the application
	USBComStatusCB m_DatalogExceptionCB 		= nullptr;
	USBComCommandCB m_SetCommandCB 				= nullptr;
	USBComCommandCB m_GetCommandCB 				= nullptr;
	USBComDataCB m_LiveDatalogCB 				= nullptr;

	// Public member functions
	RobComUSBDefaults();
	RobComUSBDefaults(CONFIG_USB config);
	virtual ~RobComUSBDefaults();

	void ConnectDevice(CONFIG_USB config);
	int InitializeVariablesByUSB(RAMVariables* RAMVarPtr, EEPROMVariables* EEVarPtr);
	int GetVariable(unsigned short memoryType, unsigned short varNr);
	int GenericGETRequest(unsigned short commandID, unsigned short value);
	int StartLiveDatalog(uint8_t varCount, uint16_t* varArrayPtr, uint16_t sampleTimeMS);
	int StopLiveDatalog(void);

	// Template functions
	// Set Variable request
	template <typename T>
	int SetVariable(unsigned short memoryType, unsigned short varNr, T varVal)
	{
		int errorCode = 0;

		// Submit the command
		errorCode = m_RobComUSB->SubmitSETCommand (memoryType, varNr, varVal);

		return errorCode;
	}

	template <typename ... argtype>
	int GenericSETRequest(unsigned short commandID, unsigned short value, argtype ... args)
	{
		int errorCode = 0;

		// Submit the command
		errorCode = m_RobComUSB->SubmitSETCommand (commandID, value, args...);

		return errorCode;
	}

private:

	Timeout *m_liveDatalogTimer;
	TimerCB m_liveDatalogTimerCB;
	std::mutex m_timerStopMutex;

	// Private member functions
	void CreateRobComUSB(CONFIG_USB config);
	void LiveDatalogGetData(void);
};


#endif /* ROBCOMUSBDEFAULTS_H_ */
