/*
 * main.cpp
 *
 *  Created on: 19.08.2019
 *      Author: roman
 */

#include <iostream>
#include <chrono>
#include "main.h"
#include "Includes/Variables.h"
// #include "Includes/RobComUSB.h"
// #include "Includes/RobComUSBDefaults.h"
// #include "Includes/USBInterface.h"
#include "DebugMessages.h"

using namespace std;

// TestClass::TestClass ()
// {
// 	EEPtr = new EEPROMVariables(21);
// 	RAMPtr = new RAMVariables(1);
// }

// TestClass::~TestClass()
// {
// 	delete EEPtr;
// 	delete RAMPtr;
// }

// void TestClass::TestOnConnectionCB(int status)
// {
// 	cout << "Hello from CB! Error Code: " << status << endl;
// }

// void TestClass::TestGetCommandCallback(void* pSetup, unsigned char* buf_ptr, int status)
// {
// 	/*
// 	for(int i = 0; i < 20; i++)
// 	   {
// 	       printf("%x",*buf_ptr++);
// 	   }
// 	   cout << endl;
// 	   cout << endl;
// 	   cout << status << endl;*/
// }

// void TestClass::TestLiveDatalogStartCommandCB(int status)
// {
// 	if(!status)
// 	{
// 		cout << "Start Live Datalog Command Callback submission successful!" << endl;
// 	}
// 	else
// 		cout << "Start Live Datalog Command Callback submission failed!" << endl;
// }

// void TestClass::TestLiveDatalogStopCommandCB(int status)
// {
// 	if(!status)
// 	{
// 		cout << "Stop Live Datalog Command Callback submission successful!" << endl;
// 	}
// 	else
// 		cout << "Stop Live Datalog Command Callback submission failed!" << endl;
// }

// void TestClass::TestMemoryReadoutCommandCB(int status)
// {

// }

// void TestClass::TestDataCB(uint8_t *bufPtr, int datalength)
// {
// 	int i = 0;

// 	if(bufPtr)
// 	{
// 		while(i<datalength)
// 		{
// 			cout << static_cast<int>(bufPtr[i]) << " ";
// 			i++;
// 		}
// 		cout << endl;
// 		i = 0;
// 	}
// }

// void TestClass::TestOnVariableInitCB(int status)
// {
// 	int i = 0;
// 	int j = 0;
// 	EEPROMVAR testvar;

// 	testvar = EEPtr->GetVarByNumber(32);

// 	/*
// 	cout << (*RAMPtr)[i].RAMVarNr << endl;
// 	cout << (*RAMPtr)[i].RAMVarValue << endl;
// 	cout << (*RAMPtr)[i].RAMVarMaxVal << endl;
// 	cout << (*RAMPtr)[i].RAMVarMinVal << endl;
// 	for(j=0;j<32;j++)
// 	{
// 		cout << (*RAMPtr)[i].description[j];
// 	}
// 	cout << endl;*/


// 	cout << testvar.EEPROMVarNr << endl;
// 	cout << testvar.EEPROMVarValue << endl;
// 	cout << testvar.EEPROMVarMaxVal << endl;
// 	cout << testvar.EEPROMVarMinVal << endl;
// 	for(j=0;j<32;j++)
// 	{
// 		cout << testvar.description[j];
// 	}
// 	cout << endl;

// 	EEPtr->SetVarByNumber(32, 3.1415926);

// 	testvar = EEPtr->GetVarByNumber(32);
// 	cout << testvar.EEPROMVarNr << endl;
// 	cout << testvar.EEPROMVarValue << endl;
// 	cout << testvar.EEPROMVarMaxVal << endl;
// 	cout << testvar.EEPROMVarMinVal << endl;
// 	for(j=0;j<32;j++)
// 	{
// 		cout << testvar.description[j];
// 	}
// 	cout << endl;

// }


int main()
{

	// uint16_t vararray[15] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	// //vararray[0]=1;
	// //int count = 0;
	// uint8_t *bufferPtr;

	// char i = 0;
	// CONFIG_USB config;

	// RobComUSBDefaults *pInterface = nullptr;
	// TestClass* pTestClass = new TestClass;

	// config.pid = 0;
	// config.vid = 0;
	// config.interface = 0;
	// config.OnConnectionCB = bind(TestClass::TestOnConnectionCB, pTestClass, placeholders::_1);
	// config.GetCommandCB = bind(TestClass::TestGetCommandCallback, pTestClass, placeholders::_1, placeholders::_2, placeholders::_3);
	// config.SetCommandCB = nullptr;
	// config.OnDisconnectionCB = bind(TestClass::TestOnConnectionCB, pTestClass, placeholders::_1);
	// config.LiveDatalogCB = bind(TestClass::TestDataCB, pTestClass, placeholders::_1, placeholders::_2);
	// config.DatalogExceptionCB = bind(TestClass::TestMemoryReadoutCommandCB, pTestClass, placeholders::_1);
	// config.DeviceSpecificationCB = nullptr;
	// config.OnVariablesInitCB = bind(TestClass::TestOnVariableInitCB, pTestClass, placeholders::_1);

	// // Create the USB Interface
	// pInterface = new RobComUSBDefaults(config);

	// // Register a GET Command
	// if(pInterface->m_RobComUSB->m_USBConnectionState == CONNECTED)
	// {
	// 	cout << "Wow! Successfully connected!" << endl;
	// }
	// //pInterface->StartLiveDatalog(1,&vararray[0],2);
	// pInterface->InitializeVariablesByUSB(pTestClass->RAMPtr, pTestClass->EEPtr);

	// this_thread::sleep_for(std::chrono::milliseconds(1000));

	// /*
	// cout << RAMVars[0].RAMVarValue << endl;
	// for (i = 0; i < 21; i++)
	// 	cout << EEVars[i].EEPROMVarValue << endl;
	// */
	// //pInterface->StopLiveDatalog();

	// //this_thread::sleep_for(std::chrono::milliseconds(2000));

	// delete pInterface;
	// delete pTestClass;
	DebugMessages *msgs = new DebugMessages();
	uint8_t *buffer;

	bool connectionEstablished = msgs->establishConnection(5, 115384);

	if (!connectionEstablished)
		cout << "Connection could not be established" << endl;
	else
		cout << "Yeah! Connection!" << endl;

	while(connectionEstablished)
	{
		if (msgs->m_msgRingBuffer->GetElement(&buffer))
			cout << buffer << endl;

		for(int i = 0; i < 100000000; i++)
			NOP_FUNCTION;

		cout << "Still alive" << endl;
		
	}

	return 0;
}


