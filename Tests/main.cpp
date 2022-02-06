/*
 * main.cpp
 *
 *  Created on: 19.08.2019
 *      Author: roman
 */

#include <iostream>
#include <sys/time.h>
#include <chrono>
#include "main.h"
#include "DebugRobComShared.h"
#include "RobCom.h"
#include <stdio.h>
#include <string>

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
	TestRingbuffer();

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

	// uint8_t *buffer;
	// uint32_t received;
	// time_t start = time(nullptr);
	// time_t elapsed;
	// RobCom *robComInst = RobComInit();
	// std::string delimiter = "\n";
	// try
	// {
	// 	debugMsgInterface(robComInst, 3, 115384);
	// 	cout << "Yeah! Connection!" << endl;
	// }
	// catch (const char *msg)
	// {
	// 	cout << msg << endl;
	// 	delete robComInst;
	// 	return 1;
	// }
	
	// while(true)
	// {
	// 	elapsed = time(nullptr) - start;

	// 	if (elapsed >= 15)
	// 		break;

	// 	received = getDebugMsg(robComInst, &buffer);

	// 	if (received > 0)
	// 	{
	// 		std::string s = reinterpret_cast<char*>(buffer);
	// 		std::string msg = s.substr(0,s.find(delimiter));

	// 		cout << msg << endl;
	// 	}

	// 	this_thread::sleep_for(std::chrono::milliseconds(10));
	// }
	// //robComInst->m_debugMessages->m_commInterface->configureReadTimeouts(10,200,2);
	// deleteRobCom(robComInst);

	// return 0;
}


