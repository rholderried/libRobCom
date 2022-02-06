/*******************************************************************************
 * RoBoardCom.cpp
 *
 * Class members of RoBoardCom
 *
 *******************************************************************************
 * Date of creation: 31.10.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 *******************************************************************************
 * TODO Migrate the connectionstate to the lower-level RobComUSB
 ******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include <RobComUSBDefaults.h>
#include "USBInterface.h"
#include "RobComUSB.h"
#include "Helpers.h"

using namespace std;


/******************************************************************************
* Defines
******************************************************************************/


/******************************************************************************
* Member function definitions
******************************************************************************/
// Default Constructor
RobComUSBDefaults::RobComUSBDefaults()
{
	// Create the timeout object for the live datalog
	m_liveDatalogTimer = new Timeout;
	m_liveDatalogTimerState = TIMER_STOPPED;
	// Initialize the timer Callback
	m_liveDatalogTimer->m_timerCallback = bind(LiveDatalogGetData, this);
}

// Constructor
RobComUSBDefaults::RobComUSBDefaults(CONFIG_USB config)
{
	// Create the timeout object for the live datalog
	m_liveDatalogTimer = new Timeout;
	m_liveDatalogTimerState = TIMER_STOPPED;
	// Initialize the timer Callback
	m_liveDatalogTimer->m_timerCallback = bind(LiveDatalogGetData, this);

	// Create the interface
	CreateRobComUSB(config);
}

// Destructor
RobComUSBDefaults::~RobComUSBDefaults(void)
{

	// Stop the live datalog timer if it is still running
	{
		lock_guard<mutex> lock(m_timerStopMutex);
		if (m_liveDatalogTimerState == TIMER_RUNNING)
		{
			m_liveDatalogTimerState = TIMER_STOP_REQUEST;
		}
	}
	// Wait until the timeout has been stopped
	while (m_liveDatalogTimerState != TIMER_STOPPED)
		;

	cout << "Delete RobComUSB" << endl;
	// Delete RobCom
	if(m_RobComUSB)
		delete m_RobComUSB;

	// Delete the timer object
	delete m_liveDatalogTimer;
}

// Try to Connect to the Device
void RobComUSBDefaults::ConnectDevice(CONFIG_USB config)
{
	// Is there a RobCom object already?
	if(m_RobComUSB)
	{
		m_RobComUSB->ConnectDevice();
	}
	// if there is no object existing, create one
	else
		CreateRobComUSB(config);
}

int RobComUSBDefaults::InitializeVariablesByUSB(RAMVariables* RAMVarPtr, EEPROMVariables* EEVarPtr)
{
	int errorCode = 0;

	// Submit the command
	errorCode = m_RobComUSB->InitVars(RAMVarPtr, EEVarPtr);

	return errorCode;
}

// Timer Callback for the live Datalogger
void RobComUSBDefaults::LiveDatalogGetData(void)
{
	unsigned char bufPtr[LIVE_DATALOG_BUFFER_WIDTH];
	int noOfElementsLeft;

	// Drain data from the ringbuffer (if any)
	while(m_RobComUSB->m_liveDatalogBuffer->GetElement(bufPtr))
	{
		// Compute the remaining elements
		noOfElementsLeft = m_RobComUSB->m_liveDatalogBuffer->m_bufSize -
				m_RobComUSB->m_liveDatalogBuffer->m_bufSpace;
		// Call the user defined callback
		m_LiveDatalogCB(bufPtr, noOfElementsLeft);
	}

	// If the datalogger is still running, set new timeout
	lock_guard<mutex> lock(m_timerStopMutex);
	if(m_liveDatalogTimerState == TIMER_RUNNING && m_RobComUSB->m_liveDatalogState != IDLE)
		m_liveDatalogTimer->SetTimeout(LIVE_DATALOG_DATA_TIMEOUT_MS, true);
	else
		m_liveDatalogTimerState = TIMER_STOPPED;
}

// Create the RobCom object
void RobComUSBDefaults::CreateRobComUSB(CONFIG_USB config)
{
	// take the callbacks
	m_DatalogExceptionCB = config.DatalogExceptionCB;
	m_SetCommandCB = config.SetCommandCB;
	m_GetCommandCB = config.GetCommandCB;
	m_LiveDatalogCB = config.LiveDatalogCB;

	m_RobComUSB = new RobComUSB({static_cast<uint16_t>(config.pid), static_cast<uint16_t>(config.vid), config.interface});

	// Configure the RobCom Connection CBs
	m_RobComUSB->m_OnConnectionCB = 	config.OnConnectionCB;
	m_RobComUSB->m_OnDisconnectionCB = 	config.OnDisconnectionCB;
	m_RobComUSB->m_DeviceSpecCB = config.DeviceSpecificationCB;
	m_RobComUSB->m_OnVarInitializeCB = config.OnVariablesInitCB;

	// Try to connect a device
	m_RobComUSB->ConnectDevice();

	// Initialize the Set and Get Commands
	if (m_RobComUSB->m_USBConnectionState == CONNECTED)
	{
		if (m_SetCommandCB)
			m_RobComUSB->RegisterSETCommand(m_SetCommandCB);
		else
			cout << "No SET Command functionality registered!" << endl;

		if (m_GetCommandCB)
			m_RobComUSB->RegisterGETCommand(m_GetCommandCB);
		else
			cout << "No GET Command functionality registered!" << endl;

		// Initialize the Datalogger
		if (m_DatalogExceptionCB && m_LiveDatalogCB)
			m_RobComUSB->RegisterDatalogTransfers(m_DatalogExceptionCB);
		else
			cout << "No Datalog functionality registered!" << endl;
	}
}

// Get variable request
int RobComUSBDefaults::GetVariable(unsigned short memoryType, unsigned short varNr)
{
	int errorCode = 0;

	// Submit the command
	errorCode = m_RobComUSB->SubmitGETCommand(memoryType, varNr);

	return errorCode;
}

// Generic GET Request
int RobComUSBDefaults::GenericGETRequest(unsigned short commandID, unsigned short value)
{
	int errorCode = 0;

	// Submit the command
	errorCode = m_RobComUSB->SubmitGETCommand(commandID, value);

	return errorCode;
}

// Start the live datalogger
int RobComUSBDefaults::StartLiveDatalog(uint8_t varCount, uint16_t* varArrayPtr, uint16_t sampleTimeMS)
{
	int errorCode = 0;

	errorCode = m_RobComUSB->StartLiveDatalog(varCount, varArrayPtr, sampleTimeMS);
	// Check the connection state
	if (!errorCode)
	{
		// Start the timer for the live datalog data getter
		m_liveDatalogTimer->SetTimeout(LIVE_DATALOG_DATA_TIMEOUT_MS, true);
		// Switch the timer state
		m_liveDatalogTimerState = TIMER_RUNNING;
	}

	return errorCode;
}

// Stop the live datalogger
int RobComUSBDefaults::StopLiveDatalog(void)
{
	int errorCode = 0;

	errorCode = m_RobComUSB->StopLiveDatalog();

	return errorCode;
}

