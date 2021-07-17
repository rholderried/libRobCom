/*******************************************************************************
 * main.h header file
 *
 *******************************************************************************
 * Date of creation: 17.10.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 ******************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "Includes/Variables.h"

/*******************************************************************************
 * Typedefs
 ******************************************************************************/


/*******************************************************************************
 * Classes
 ******************************************************************************/
class TestClass
{

public:
	EEPROMVariables* EEPtr;
	RAMVariables* RAMPtr;

	// Public member functions
	TestClass();
	void TestOnConnectionCB(int status);
	void TestDataCB(uint8_t *bufPtr, int datalength);
	void TestGetCommandCallback(void* pSetup, unsigned char* buf_ptr, int status);
	void TestLiveDatalogStartCommandCB(int status);
	void TestLiveDatalogStopCommandCB(int status);
	void TestMemoryReadoutCommandCB(int status);
	void TestOnVariableInitCB(int status);
	~TestClass();
};


#endif /* MAIN_H_ */
