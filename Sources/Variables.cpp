/*******************************************************************************
 * Variables.cpp
 *
 * Member function definitions of the Variables class.
 *
 *******************************************************************************
 * Date of creation: 10.11.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 *******************************************************************************
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "Variables.h"

using namespace std;

/*******************************************************************************
 * EEPROMVariables -> Member function definitions
 ******************************************************************************/
// Constructor
EEPROMVariables::EEPROMVariables(int noOfVars)
{
	m_numberOfVars = noOfVars;

	m_EEPROMVars = new vector<EEPROMVARMEMBER>(noOfVars);
	// Initialize helpers
	// m_helPtr = new HelperFunctions();
}


// Destructor
EEPROMVariables::~EEPROMVariables(void)
{
	delete m_EEPROMVars;
	//delete m_helPtr;
}

// Initialize an EEPROM Variable
int EEPROMVariables::InitVarByNumber(EEPROMVAR varInitializer)
{
	int errorCode = 0;
	unsigned int i = 0;
	EEPROMVARMEMBER tmp;

	// Find the right place to insert the variable
	while ((*m_EEPROMVars)[i].variable.EEPROMVarNr < varInitializer.EEPROMVarNr &&
		   (*m_EEPROMVars)[i].variable.EEPROMVarNr > 0)
		i++;

	if (i > m_numberOfVars)
		errorCode = 1; // TODO: Define Error on extensive data
	else
	{
		// Create temporary data
		tmp.variable = varInitializer;
		tmp.initialized = true;
		// insert the data in order to get it sorted
		m_EEPROMVars->insert(m_EEPROMVars->begin()+ i,tmp);
	}

	return errorCode;
}

// Get the value of a variable indexed by its number
EEPROMVAR EEPROMVariables::GetVarByNumber(unsigned short varNr)
{
	unsigned short i = 0;
	EEPROMVAR retVal;

	// Search for the var nr
	while(i < m_numberOfVars)
	{
		if ((*m_EEPROMVars)[i].variable.EEPROMVarNr ==  varNr)
			break;
		i++;
	}
	// Get the value if there is a var with this number, otherwise return 0
	if(i < m_numberOfVars)
		retVal = (*m_EEPROMVars)[i].variable;

	return retVal;
}

// Set the value of a variable indexed by its number
int EEPROMVariables::SetVarByNumber(unsigned short varNr, double value)
{
	unsigned short i = 0;
	int errorCode = 0;

	// Search for the var nr
	while(i < m_numberOfVars)
	{
		if ((*m_EEPROMVars)[i].variable.EEPROMVarNr ==  varNr)
			break;
		i++;
	}
	// Get the value if there is a var with this number, otherwise return 0
	if(i < m_numberOfVars)
		(*m_EEPROMVars)[i].variable.EEPROMVarValue = value;
	else
		errorCode = 1; // TODO: Define Error Code on failed var write

	return errorCode;
}


/*******************************************************************************
 * RAMVariables -> Member function definitions
 ******************************************************************************/
// Constructor
RAMVariables::RAMVariables(int noOfVars)
{
	m_numberOfVars = noOfVars;

	m_RAMVars = new vector<RAMVARMEMBER>(noOfVars);
	// Initialize helpers
	// m_helPtr = new HelperFunctions();
}


// Destructor
RAMVariables::~RAMVariables(void)
{
	delete m_RAMVars;
	//delete m_helPtr;
}

// Initialize a RAM Variable
int RAMVariables::InitVarByNumber(RAMVAR varInitializer)
{
	int errorCode = 0;
	unsigned int i = 0;
	RAMVARMEMBER tmp;

	// Find the right place to insert the variable
	while ((*m_RAMVars)[i].variable.RAMVarNr < varInitializer.RAMVarNr &&
		   (*m_RAMVars)[i].variable.RAMVarNr > 0)
		i++;

	if (i > m_numberOfVars)
		errorCode = 1; // TODO: Define Error on extensive data
	else
	{
		// Create temporary data
		tmp.variable = varInitializer;
		tmp.initialized = true;
		// insert the data in order to get it sorted
		m_RAMVars->insert(m_RAMVars->begin()+ i,tmp);
	}

	return errorCode;
}

// Get the value of a variable indexed by its number
RAMVAR RAMVariables::GetVarByNumber(unsigned short varNr)
{
	unsigned short i = 0;
	RAMVAR retVal;

	// Search for the var nr
	while(i < m_numberOfVars)
	{
		if ((*m_RAMVars)[i].variable.RAMVarNr ==  varNr)
			break;
		i++;
	}
	// Get the value if there is a var with this number, otherwise return 0
	if(i < m_numberOfVars)
		retVal = (*m_RAMVars)[i].variable;

	return retVal;
}

// Set the value of a variable indexed by its number
int RAMVariables::SetVarByNumber(unsigned short varNr, double value)
{
	unsigned short i = 0;
	int errorCode = 0;

	// Search for the var nr
	while(i < m_numberOfVars)
	{
		if ((*m_RAMVars)[i].variable.RAMVarNr ==  varNr)
			break;
		i++;
	}
	// Get the value if there is a var with this number, otherwise return 0
	if(i < m_numberOfVars)
		(*m_RAMVars)[i].variable.RAMVarValue = value;
	else
		errorCode = 1; // TODO: Define Error Code on failed var write

	return errorCode;
}

//
