/*******************************************************************************
 * RobComUSB.cpp
 *
 * Member function definitions of the RobComUSB class.
 *
 *******************************************************************************
 * Date of creation: 17.10.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 *******************************************************************************
 * TODO We need to add versioning to the Project.
 ******************************************************************************/

#include <RobComUSB.h>
#include "USBInterface.h"
#include "Helpers.h"
#include "Variables.h"
#include <iostream>


using namespace std;

// Minimal Constructor
RobComUSB::RobComUSB(COMCONFIG comConfig)
{
	// Initializing product ID, vendor ID and Interface
	m_comConfig = comConfig;
	m_datalogExceptionCB = nullptr;

	// Initialize the communication interfaces
	m_ifPtr = new USBInterface();
	m_ifPtr->m_onConnectionCB = bind(OnConnectionStateChange, this, placeholders::_1);
	// Initialize helpers
	m_helPtr = new HelperFunctions();
	// Initialize the live datalog ringbuffer structure
	m_liveDatalogBuffer = new Ringbuffer<uint8_t>(LIVE_DATALOG_BUFFER_SIZE, LIVE_DATALOG_BUFFER_WIDTH);

	m_liveDatalogTimeoutPtr = new Timeout();
	m_liveDatalogTimeoutPtr->m_timerCallback = bind(LiveDatalogSendDataRequest,this);
}

// USB device connection
int RobComUSB::ConnectDevice(void)
{
	ROBCOMUSBERROR error_code = ERROR_NONE;

	if (m_USBConnectionState == CONNECTED)
		error_code = USB_ERROR_DEVICE_ALREADY_CONFIGURED;
	else
	{
		cout << "Connecting USB Device" << endl;
		error_code = static_cast<ROBCOMUSBERROR>(m_ifPtr->InitializeFunctionality(m_comConfig.pid, m_comConfig.vid, m_comConfig.interface));
	}

	if(error_code)
		cout << "Device connection failed!" << endl;
	else
		cout << "Device connection successful!" << endl;


	return(static_cast<int>(error_code));
}

// GET Command registration
int RobComUSB::RegisterGETCommand(CommandCallback cbFunction)
{
	int errorCode = 0;

	if (cbFunction != nullptr)
	{
		// Check if transfer is already in use
		if (!(m_ifPtr->m_transfer_array_indizes & (1 << GET_COMMAND_IDX)))
			RegisterBidirectionalCommand(&m_getCommandBuffer[0], GET_COMMAND_BREQUEST, static_cast<uint8_t>(GET_COMMAND_IDX),
										 bind(GetCommandCB,this,placeholders::_1,placeholders::_2,placeholders::_3));

		// Save the callback function
		m_GetCmdCB = cbFunction;
	}
	else
		errorCode = 1; // TODO Define Error on failed Get Command Registration

	return errorCode;
}

// SET Command registration
int RobComUSB::RegisterSETCommand(CommandCallback cbFunction)
{
	int errorCode = 0;

	if (cbFunction != nullptr)
	{
		// Check if transfer is already in use
		if (!(m_ifPtr->m_transfer_array_indizes & (1 << SET_COMMAND_IDX)))
			RegisterUnidirectionalCommand(&m_setCommandBuffer[0], SET_COMMAND_BREQUEST, static_cast<uint8_t>(SET_COMMAND_IDX),
										  bind(SetCommandCB,this,placeholders::_1,placeholders::_2,placeholders::_3));

		// Save the callback function
		m_SetCmdCB = cbFunction;
	}
	else
		errorCode = 1; // TODO Define Error on failed Set Command Registration

	return errorCode;
}

// Bidirectional Command registration
void RobComUSB::RegisterBidirectionalCommand(uint8_t* bufPtr, uint8_t requestID, uint8_t commandIdx, CommandCallback cbFunction)
{
	// TODO: Transfer Index management has to be done here
	m_ifPtr->RegisterCtrlTransfer(bufPtr, CONTROL_TRANSFER_BIDIRECTIONAL, requestID, CONTROL_TRANSFER_TIMEOUT_MS,
		                     	 CONTROL_TRANSFER_BUFFER_SZ,commandIdx, cbFunction);

}

// Unidirectional Command registration
void RobComUSB::RegisterUnidirectionalCommand(uint8_t* bufPtr, uint8_t requestID, uint8_t commandIdx, CommandCallback cbFunction)
{
	// TODO: Transfer Index management has to be done here
	m_ifPtr->RegisterCtrlTransfer(bufPtr, CONTROL_TRANSFER_UNIDIRECTIONAL, requestID, CONTROL_TRANSFER_TIMEOUT_MS,
			                      CONTROL_TRANSFER_BUFFER_SZ, commandIdx, cbFunction);
}

// Register all Datalog Transfers
void RobComUSB::RegisterDatalogTransfers(StatusCallback datalogExceptionCB)
{
	// Assign Callbacks
	m_datalogExceptionCB = datalogExceptionCB;

	// Register Transfers
	// Internal Datalog Init Command
	RegisterUnidirectionalCommand(&m_startMemoryReadoutCommandBuffer[0], SET_COMMAND_BREQUEST,
								 static_cast<uint8_t>(DATALOG_MEMORY_READOUT_COMMAND_IDX),
								 bind(DatalogCommandCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// Start Internal Datalog Command
	RegisterUnidirectionalCommand(&m_startMemoryReadoutCommandBuffer[0], SET_COMMAND_BREQUEST,
								 static_cast<uint8_t>(DATALOG_INTERNAL_START_COMMAND_IDX),
								 bind(DatalogCommandCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// Start Internal Datalog Command
	RegisterUnidirectionalCommand(&m_startMemoryReadoutCommandBuffer[0], SET_COMMAND_BREQUEST,
								 static_cast<uint8_t>(DATALOG_INTERNAL_STOP_COMMAND_IDX),
								 bind(DatalogCommandCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// Set Internal Datalog from Memory
	RegisterUnidirectionalCommand(&m_startMemoryReadoutCommandBuffer[0], SET_COMMAND_BREQUEST,
								 static_cast<uint8_t>(DATALOG_INTERNAL_SET_FROM_MEMORY_COMMAND_IDX),
								 bind(DatalogCommandCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// Start Live Datalog
	RegisterUnidirectionalCommand(&m_startMemoryReadoutCommandBuffer[0], SET_COMMAND_BREQUEST,
								 static_cast<uint8_t>(DATALOG_LIVE_START_COMMAND_IDX),
								 bind(DatalogCommandCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// Stop Live Datalog
	RegisterUnidirectionalCommand(&m_startMemoryReadoutCommandBuffer[0], SET_COMMAND_BREQUEST,
								 static_cast<uint8_t>(DATALOG_LIVE_STOP_COMMAND_IDX),
								 bind(DatalogCommandCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// Start Memory Readout Command
	RegisterUnidirectionalCommand(&m_startMemoryReadoutCommandBuffer[0], SET_COMMAND_BREQUEST,
								 static_cast<uint8_t>(DATALOG_MEMORY_READOUT_COMMAND_IDX),
								 bind(DatalogCommandCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// DATA IN Bulk transfer for the memory readout
	m_ifPtr->RegisterBulkTransfer(&m_memoryReadoutBuffer[0], DATA_IN_EP_ADDRESS, MEMORY_READOUT_TIMEOUT_MS,
								  MEMORY_READOUT_BUFFER_SZ, DATALOG_MEMORY_READOUT_DATA_IDX,
								  bind(MemoryReadoutLowLevelCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));

	// DATA IN Bulk transfer for the live datalogger
	m_ifPtr->RegisterBulkTransfer(&m_liveDatalogBuffer->m_ringbuffer[0], DATA_IN_EP_ADDRESS, MEMORY_READOUT_TIMEOUT_MS,
								  LIVE_DATALOG_BUFFER_SIZE, DATALOG_LIVE_DATA_IDX,
								  bind(LiveDatalogLowLevelCB, this, placeholders::_1, placeholders::_2, placeholders::_3 ));
}

// Sync the application with data from the device
int RobComUSB::GetDeviceSpecification(void)
{
	int errorCode = 0;

	if(m_USBConnectionState != CONNECTED)
		errorCode = 1;
	else
	{
		// Request Board infos
		SubmitGETCommand(GET_DEVICE_DESCRIPTION,0);
	}
	return errorCode;
}

// Initialize variable structures
int RobComUSB::InitVars(RAMVariables* RAMVarPtr, EEPROMVariables* EEPROMVarPtr)
{
	int errorCode = 0;

	if(m_USBConnectionState != CONNECTED)
		errorCode = 1; // TODO Define Error on not Connected
	else
	{
		m_RAMVariables = RAMVarPtr;
		m_EEPROMVariables = EEPROMVarPtr;
		m_RAMVarCount = RAMVarPtr->m_numberOfVars;
		m_EEPROMVarCount = EEPROMVarPtr->m_numberOfVars;

		// Get the first variable
		if(m_RAMVariables > 0)
			SubmitGETCommand(GET_RAM_VAR_DESC,m_RAMVarCount-1);
		else if(m_EEPROMVarCount > 0)
			SubmitGETCommand(GET_EEPROM_VAR_DESC,m_EEPROMVarCount-1);
	}
	return errorCode;
}

// Start the Live Datalogger
int RobComUSB::StartLiveDatalog(uint8_t varCount, uint16_t* varArrayPtr, uint16_t sampleTimeMS)
{
	uint8_t tmpBuf[35] = {0};
	size_t dataSize = 0;
	int errorCode = 0;
	if (m_USBConnectionState != CONNECTED)
		errorCode = 1; // TODO: Define errors!
	else
	{
		// TODO: Define Error for error on already running datalog
		if (m_liveDatalogState == IDLE)
		{
			// Delete all still residing data in the ringbuffer
			while(m_liveDatalogBuffer->DeleteLastData())
				;

			// Call the helper to fill the Buffer
			dataSize = m_helPtr->WriteUI8Buffer(&tmpBuf[0], varCount, varArrayPtr[0], varArrayPtr[1], varArrayPtr[2], varArrayPtr[3], varArrayPtr[4], varArrayPtr[5],
												varArrayPtr[6], varArrayPtr[7], varArrayPtr[8], varArrayPtr[9], varArrayPtr[10], varArrayPtr[11], varArrayPtr[12],
												varArrayPtr[13], varArrayPtr[14], sampleTimeMS);

			// Submit the SET Command Request
			errorCode = SubmitCommand(&tmpBuf[0], dataSize, static_cast<uint16_t>(DATALOG_LIVE_START), 0, DATALOG_LIVE_START_COMMAND_IDX);

			m_liveDatalogState = START_REQUEST;

			m_liveDatalogSampleTimeMS = sampleTimeMS;
		}
		else
			errorCode = 2;
	}

	return errorCode;
}

// Stop the Live Datalogger
int RobComUSB::StopLiveDatalog (void)
{
	int errorCode = 0;

	if (m_USBConnectionState != CONNECTED)
		errorCode = 1; // TODO: Define errors!
	else
	{
		// TODO: Define Error for error on already stopped datalog
		if (m_liveDatalogState == RUNNING)
		{
			errorCode = SubmitCommand(nullptr, 0, static_cast<uint16_t>(DATALOG_LIVE_STOP), 0, DATALOG_LIVE_STOP_COMMAND_IDX);

			// Flag a stop request
			m_liveDatalogState = STOP_REQUEST;
		}
		else
			errorCode = 2;
	}

	return errorCode;
}

// SubmitGETCommand for non-argument transfers
int RobComUSB::SubmitGETCommand(uint16_t commandID, uint16_t value)
{
	int errorCode = 0;

	// Check the connection state
	if (m_USBConnectionState != CONNECTED)
		errorCode = 1; // TODO: Define errors!
	else
	{
		// Call the lower-level submit function
		errorCode = SubmitCommand(nullptr, CONTROL_TRANSFER_MAX_DATA_SZ, commandID, value, static_cast<uint8_t>(GET_COMMAND_IDX));
	}

	return errorCode;
}

// Command for the Memory Readout
int RobComUSB::SubmitMemoryReadoutCommand(uint32_t memoryOffset, uint32_t lastAddress)
{
	int errorCode = 0;
	uint8_t buffer[8];
	size_t dataSize;
	if (m_USBConnectionState != CONNECTED)
		errorCode = 1; // TODO: Define errors!
	else
	{
		// Write buffer
		dataSize = m_helPtr->WriteUI8Buffer(buffer, memoryOffset, lastAddress);

		// Call the lower-level submit function
		errorCode = SubmitCommand(&buffer[0], static_cast<uint16_t>(dataSize), DATALOG_READ_MEMORY, 0, static_cast<uint8_t>(DATALOG_MEMORY_READOUT_COMMAND_IDX));
	}

	return errorCode;
}

// Command Request verschicken
int RobComUSB::SubmitCommand(uint8_t* bufPtr, uint16_t dataSize, uint16_t commandID, uint16_t value, uint8_t transferIdx)
{
    int errorCode;

    // Ebenso zurück, wenn die Datenlänge größer als 64 Byte ist.
    if (dataSize > 64)
        return static_cast<int>(USB_ERROR_SUBMISSION_TOO_MUCH_DATA);

    // Setup-Paket ändern
    m_ifPtr->EditCtrlTransfer(bufPtr, commandID, value, dataSize, static_cast<uint8_t>(transferIdx));

    // Stay thread save for the submission process
    lock_guard<mutex> lock(m_submitMutex);
    errorCode = m_ifPtr->SubmitTransfer(transferIdx);

    return errorCode;
}

// This function gets called on every change of the connection state
void RobComUSB::OnConnectionStateChange(NEWCONNECTIONSTATE newConnectionState)
{

switch (newConnectionState)
{
case DEVICE_CONNECTED:
	m_USBConnectionState = CONNECTED;

	// Call the external OnDisconnectionCB
	if (m_OnConnectionCB)
		m_OnConnectionCB(static_cast<int>(m_USBConnectionState));
	break;

case DEVICE_DISCONNECTED:
	m_USBConnectionState = DISCONNECTED;

	cout << "Detected disconnect!" << endl;

	// Call the external OnDisconnectionCB
	if (m_OnDisconnectionCB)
		m_OnDisconnectionCB(static_cast<int>(m_USBConnectionState));

	// Deinitialize the USBInterface. This has to be done in another thread,
	// because this function is likely to be called from the event handler
	// thread.
	thread([this]()
	{
		this->m_ifPtr->DeinitializeFunctionality();
	}).detach();

	break;

default:
	break;
}
}

// Get Command Callback
void RobComUSB::GetCommandCB(void* pSetup, uint8_t* bufPtr, int status)
{
	uint8_t i = 0;
	CONTROL_SETUP* pSetupInt = reinterpret_cast<CONTROL_SETUP*>(pSetup);
	RAMVAR		tmpRAMVar;
	EEPROMVAR	tmpEEPROMVar;
	// Check if this was a default ID request
	if(!status)
	{
		switch (static_cast<DEFAULT_GET_CMD_ID>(pSetupInt->wIndex))
		{
		case GET_NONE:
			break;

		case GET_EEPROM_VARIABLE:
			// TODO Get EEPROM Variable
			break;
		case GET_RAM_VARIABLE:
			// TODO Get RAM Variable
			break;

		case GET_DEVICE_DESCRIPTION:
			// Call the user specified callback
			if(m_DeviceSpecCB)
				m_DeviceSpecCB(bufPtr, status);

			break;

		case GET_RAM_VAR_DESC:
			tmpRAMVar.RAMVarNr = m_helPtr->ConvertUI8BufferData<uint16_t, uint16_t>(bufPtr);
			bufPtr+=2;
			tmpRAMVar.RAMVarType = static_cast<VAR_TYPE>(*bufPtr++);

			// Save Variable data according to the data type
			switch(tmpRAMVar.RAMVarType)
			{
			case TYPE_UI8:
				tmpRAMVar.RAMVarValue = m_helPtr->ConvertUI8BufferData<uint8_t, double>(bufPtr++);
				tmpRAMVar.RAMVarMaxVal = m_helPtr->ConvertUI8BufferData<uint8_t, double>(bufPtr++);
				tmpRAMVar.RAMVarMinVal = m_helPtr->ConvertUI8BufferData<uint8_t, double>(bufPtr++);
				break;
			case TYPE_I8:
				tmpRAMVar.RAMVarValue = m_helPtr->ConvertUI8BufferData<int8_t, double>(bufPtr++);
				tmpRAMVar.RAMVarMaxVal = m_helPtr->ConvertUI8BufferData<int8_t, double>(bufPtr++);
				tmpRAMVar.RAMVarMinVal = m_helPtr->ConvertUI8BufferData<int8_t, double>(bufPtr++);
				break;
			case TYPE_UI16:
				tmpRAMVar.RAMVarValue = m_helPtr->ConvertUI8BufferData<uint16_t, double>(bufPtr);
				bufPtr+=2;
				tmpRAMVar.RAMVarMaxVal = m_helPtr->ConvertUI8BufferData<uint16_t, double>(bufPtr);
				bufPtr+=2;
				tmpRAMVar.RAMVarMinVal = m_helPtr->ConvertUI8BufferData<uint16_t, double>(bufPtr);
				bufPtr+=2;
				break;
			case TYPE_I16:
				tmpRAMVar.RAMVarValue = m_helPtr->ConvertUI8BufferData<int16_t, double>(bufPtr);
				bufPtr+=2;
				tmpRAMVar.RAMVarMaxVal = m_helPtr->ConvertUI8BufferData<int16_t, double>(bufPtr);
				bufPtr+=2;
				tmpRAMVar.RAMVarMinVal = m_helPtr->ConvertUI8BufferData<int16_t, double>(bufPtr);
				bufPtr+=2;
				break;
			case TYPE_UI32:
				tmpRAMVar.RAMVarValue = m_helPtr->ConvertUI8BufferData<uint32_t, double>(bufPtr);
				bufPtr+=4;
				tmpRAMVar.RAMVarMaxVal = m_helPtr->ConvertUI8BufferData<uint32_t, double>(bufPtr);
				bufPtr+=4;
				tmpRAMVar.RAMVarMinVal = m_helPtr->ConvertUI8BufferData<uint32_t, double>(bufPtr);
				bufPtr+=4;
				break;
			case TYPE_I32:
				tmpRAMVar.RAMVarValue = m_helPtr->ConvertUI8BufferData<int32_t, double>(bufPtr);
				bufPtr+=4;
				tmpRAMVar.RAMVarMaxVal = m_helPtr->ConvertUI8BufferData<int32_t, double>(bufPtr);
				bufPtr+=4;
				tmpRAMVar.RAMVarMinVal = m_helPtr->ConvertUI8BufferData<int32_t, double>(bufPtr);
				bufPtr+=4;
				break;
			case TYPE_F32:
				tmpRAMVar.RAMVarValue = m_helPtr->ConvertUI8BufferData<float, double>(bufPtr);
				bufPtr+=4;
				tmpRAMVar.RAMVarMaxVal = m_helPtr->ConvertUI8BufferData<float, double>(bufPtr);
				bufPtr+=4;
				tmpRAMVar.RAMVarMinVal = m_helPtr->ConvertUI8BufferData<float, double>(bufPtr);
				bufPtr+=4;
				break;
			}

			tmpRAMVar.access = static_cast<ACCESS>(*bufPtr++);
			// Save the description
			for(i = 0; i < BYTE_LENGTH_DESCRIPTION; i++)
			{
				tmpRAMVar.description[i]=*bufPtr++;
			}

			// The var is now completely defined, so save it into the structure
			m_RAMVariables->InitVarByNumber(tmpRAMVar);

			// Get the next var, if there is any
			if(--m_RAMVarCount)
				SubmitGETCommand(GET_RAM_VAR_DESC,m_RAMVarCount-1);
			// Otherwise, get the first EEPROM Var
			else if(m_EEPROMVarCount)
				SubmitGETCommand(GET_EEPROM_VAR_DESC,m_EEPROMVarCount-1);
			// Otherwise, call the On Initialization CB
			else if(m_OnVarInitializeCB)
				m_OnVarInitializeCB(status);
			break;

		case GET_EEPROM_VAR_DESC:
			tmpEEPROMVar.EEPROMVarNr = m_helPtr->ConvertUI8BufferData<uint16_t, uint16_t>(bufPtr);
			bufPtr+=2;
			tmpEEPROMVar.EEPROMVarType = static_cast<VAR_TYPE>(*bufPtr++);

			// Save Variable data according to the data type
			switch(tmpEEPROMVar.EEPROMVarType)
			{
			case TYPE_UI8:
				tmpEEPROMVar.EEPROMVarValue = m_helPtr->ConvertUI8BufferData<uint8_t, double>(bufPtr++);
				tmpEEPROMVar.EEPROMVarDefault = m_helPtr->ConvertUI8BufferData<uint8_t, double>(bufPtr++);
				tmpEEPROMVar.EEPROMVarMaxVal = m_helPtr->ConvertUI8BufferData<uint8_t, double>(bufPtr++);
				tmpEEPROMVar.EEPROMVarMinVal = m_helPtr->ConvertUI8BufferData<uint8_t, double>(bufPtr++);
				break;
			case TYPE_I8:
				tmpEEPROMVar.EEPROMVarValue = m_helPtr->ConvertUI8BufferData<int8_t, double>(bufPtr++);
				tmpEEPROMVar.EEPROMVarDefault = m_helPtr->ConvertUI8BufferData<int8_t, double>(bufPtr++);
				tmpEEPROMVar.EEPROMVarMaxVal = m_helPtr->ConvertUI8BufferData<int8_t, double>(bufPtr++);
				tmpEEPROMVar.EEPROMVarMinVal = m_helPtr->ConvertUI8BufferData<int8_t, double>(bufPtr++);
				break;
			case TYPE_UI16:
				tmpEEPROMVar.EEPROMVarValue = m_helPtr->ConvertUI8BufferData<uint16_t, double>(bufPtr);
				bufPtr+=2;
				tmpEEPROMVar.EEPROMVarDefault = m_helPtr->ConvertUI8BufferData<uint16_t, double>(bufPtr);
				bufPtr+=2;
				tmpEEPROMVar.EEPROMVarMaxVal = m_helPtr->ConvertUI8BufferData<uint16_t, double>(bufPtr);
				bufPtr+=2;
				tmpEEPROMVar.EEPROMVarMinVal = m_helPtr->ConvertUI8BufferData<uint16_t, double>(bufPtr);
				bufPtr+=2;
				break;
			case TYPE_I16:
				tmpEEPROMVar.EEPROMVarValue = m_helPtr->ConvertUI8BufferData<int16_t, double>(bufPtr);
				bufPtr+=2;
				tmpEEPROMVar.EEPROMVarDefault = m_helPtr->ConvertUI8BufferData<int16_t, double>(bufPtr);
				bufPtr+=2;
				tmpEEPROMVar.EEPROMVarMaxVal = m_helPtr->ConvertUI8BufferData<int16_t, double>(bufPtr);
				bufPtr+=2;
				tmpEEPROMVar.EEPROMVarMinVal = m_helPtr->ConvertUI8BufferData<int16_t, double>(bufPtr);
				bufPtr+=2;
				break;
			case TYPE_UI32:
				tmpEEPROMVar.EEPROMVarValue = m_helPtr->ConvertUI8BufferData<uint32_t, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarDefault = m_helPtr->ConvertUI8BufferData<uint32_t, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarMaxVal = m_helPtr->ConvertUI8BufferData<uint32_t, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarMinVal = m_helPtr->ConvertUI8BufferData<uint32_t, double>(bufPtr);
				bufPtr+=4;
				break;
			case TYPE_I32:
				tmpEEPROMVar.EEPROMVarValue = m_helPtr->ConvertUI8BufferData<int32_t, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarDefault = m_helPtr->ConvertUI8BufferData<int32_t, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarMaxVal = m_helPtr->ConvertUI8BufferData<int32_t, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarMinVal = m_helPtr->ConvertUI8BufferData<int32_t, double>(bufPtr);
				bufPtr+=4;
				break;
			case TYPE_F32:
				tmpEEPROMVar.EEPROMVarValue = m_helPtr->ConvertUI8BufferData<float, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarDefault = m_helPtr->ConvertUI8BufferData<float, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarMaxVal = m_helPtr->ConvertUI8BufferData<float, double>(bufPtr);
				bufPtr+=4;
				tmpEEPROMVar.EEPROMVarMinVal = m_helPtr->ConvertUI8BufferData<float, double>(bufPtr);
				bufPtr+=4;
				break;
			}

			tmpEEPROMVar.access = static_cast<ACCESS>(*bufPtr++);
			// Save the description
			for(i = 0; i < BYTE_LENGTH_DESCRIPTION; i++)
			{
				tmpEEPROMVar.description[i]=*bufPtr++;
			}

			// The var is now completely defined, so save it into the structure
			m_EEPROMVariables->InitVarByNumber(tmpEEPROMVar);

			// Get the next var, if there is any
			if(--m_EEPROMVarCount)
				SubmitGETCommand(GET_EEPROM_VAR_DESC,m_EEPROMVarCount-1);
			// Otherwise, call the On Initialization CB
			else if(m_OnVarInitializeCB)
				m_OnVarInitializeCB(status);
			break;

		default:
			break;
		}
	}
	// Call the user defined callback
	m_GetCmdCB(pSetup,bufPtr,status);
}

// Set Command Callback
void RobComUSB::SetCommandCB(void* pSetup, uint8_t* bufPtr, int status)
{
	CONTROL_SETUP* pSetupInt = reinterpret_cast<CONTROL_SETUP*>(pSetup);
	// Check if this was a default ID request
	if(!status)
	{
		switch (static_cast<DEFAULT_SET_CMD_ID>(pSetupInt->wIndex))
		{
		case SET_NONE:
			break;

		case SET_EEPROM_VARIABLE:
		case SET_RAM_VARIABLE:
			// Request the var in order to read it back
			SubmitGETCommand(pSetupInt->wIndex,pSetupInt->wValue);
			// We do not call the user defined callback now
			return;

		default:
			break;
		}
	}
	// Call the user defined callback
	m_SetCmdCB(pSetup,bufPtr,status);
}

// Start the Memory readout transfer
void RobComUSB::DatalogCommandCB(void* pSetup, unsigned char* buf_ptr, int status)
{
	//ROBCOMUSBTRANSFERSTATUS transferStatus = static_cast<ROBCOMUSBTRANSFERSTATUS>(status);

	CONTROL_SETUP* pSetupInt = reinterpret_cast<CONTROL_SETUP*>(pSetup);
	// React to a successful transfer
	switch(pSetupInt->wIndex)
	{
	case DATALOG_INTERNAL_INIT:
		// Execute Callback
		break;
	case DATALOG_INTERNAL_START:
		// Execute Callback
		break;
	case DATALOG_INTERNAL_STOP:
		// Execute Callback
		break;
	case DATALOG_INTERNAL_SET_FROM_MEMORY:
		// Execute Callback
		break;
	case DATALOG_LIVE_START:
		// Execute Callback
		cout << "DATALOG_LIVE_START" << endl;
		// If the Live Datalog request wasn't successful, then switch back
		// to IDLE state. TODO: Define errors on start request failure
		if (status)
			m_liveDatalogState = IDLE;
		else
		{
			// Get the first sampled data immediately
			LiveDatalogSendDataRequest();
		}

		break;
	case DATALOG_LIVE_STOP:
			if(!status)
				m_liveDatalogState = IDLE;
			// TODO: Define errors on stop request failure
		break;

	case DATALOG_READ_MEMORY:
		// Submit Memory readout bulk transfer
		if (!status)
		{
			// Stay thread save for the submission process
			lock_guard<mutex> lock(m_submitMutex);
			m_ifPtr->SubmitTransfer(static_cast<uint8_t>(DATALOG_MEMORY_READOUT_DATA_IDX));
		}
	}
}

// TODO: Refinement
void RobComUSB::MemoryReadoutLowLevelCB(void* pSetup, unsigned char* buf_ptr, int status)
{
	//CONTROL_SETUP* pSetupInt = reinterpret_cast<CONTROL_SETUP*>(pSetup);

	int i = 0;
	cout << "MemoryDataReadyCB" << endl;
	cout << "Errorcode: " << status << endl;
	if (!status)
	{
		//----------------------------------------------------------------------
		// Debug: Display data
		while (i<MEMORY_READOUT_BUFFER_SZ)
		{
			printf("Index %d: %d",i, m_memoryReadoutBuffer[i]);
			cout << endl;
			i++;
		}
		cout << endl;
		//----------------------------------------------------------------------
	}
}

// Low level Callback for the live datalogger
void RobComUSB::LiveDatalogLowLevelCB(void* pSetup, unsigned char* buf_ptr, int status)
{
	//CONTROL_SETUP* pSetupInt = reinterpret_cast<CONTROL_SETUP*>(pSetup);

	ROBCOMUSBTRANSFERSTATUS transferStatus = static_cast<ROBCOMUSBTRANSFERSTATUS>(status);

	switch (transferStatus)
	{
	case USB_TRANSFER_COMPLETED:// Everthing's ok -> last transfer successful
		// Has the datalogger just been started?
		if (m_liveDatalogState == START_REQUEST)
		{
			// If the first transfer has been successfully completed, the
			// datalogger is considered to be in running state.
			m_liveDatalogState = RUNNING;
		}
		// Invoke next transfer
		if (m_liveDatalogState == RUNNING)
			m_liveDatalogTimeoutPtr->SetTimeout(m_liveDatalogSampleTimeMS, true);
		else if (m_liveDatalogState == STOP_REQUEST)
			m_liveDatalogState = IDLE;
		break;

	// If a timeout occures when a live datalog transfer was requested, then the
	// device did not start the datalog due to unsufficient parameters.
	case USB_TRANSFER_TIMED_OUT:
		if (m_datalogExceptionCB)
			m_datalogExceptionCB(LIVE_DATALOG_CONFIGURATION_ERROR);

		// Delete last entry in the ringbuffer
		m_liveDatalogBuffer->DeleteLastData(false);

		cout << "Timeout occured! Switching off Datalogger" << endl;
		m_liveDatalogState = IDLE;

		break;

	// The transfer is sporadically cancelled by libusb for some reason. If that
	// is the case, delete the last ringbuffer entry and continue. The exception
	// callback is not invoked in this case.
	case USB_TRANSFER_CANCELLED:
	{
		// Invoke next transfer
		m_liveDatalogTimeoutPtr->SetTimeout(m_liveDatalogSampleTimeMS, true);
		// Delete last entry in the ringbuffer
		m_liveDatalogBuffer->DeleteLastData(false);
		break;
	}

	default:
		break;
	}

	// Conclude the transfer by releasing the buffer
	m_liveDatalogBuffer->ConcludeBufferWrite();

	// Debug: Display data

	//--------------------------------------------------------------------------
}

// This function repeats the live datalog data request.
void RobComUSB::LiveDatalogSendDataRequest(void)
{
	// Repeat only if there has been no stop request.
	// Stay thread save for the submission process
	lock_guard<mutex> lock(m_submitMutex);

	// Call the Submit routine using the Live Datalog Ringbuffer structure
	m_ifPtr->SubmitTransfer(static_cast<uint8_t>(DATALOG_LIVE_DATA_IDX),
			 &m_liveDatalogBuffer->m_ringbuffer[m_liveDatalogBuffer->PrepareBufferWriteAtIndex()]);
}

RobComUSB::~RobComUSB()
{
	cout << "Deleting USB Interface" << endl;

	// if there is still a live log running, stop this first
	if (m_liveDatalogState == RUNNING)
		StopLiveDatalog();

	// Wait until the datalog has been stopped
	while (m_liveDatalogState != IDLE)
		;

	// Delete the USB interface
	if(m_ifPtr)
		delete m_ifPtr;

	delete m_helPtr;
	delete m_liveDatalogTimeoutPtr;
	delete m_liveDatalogBuffer;
}
