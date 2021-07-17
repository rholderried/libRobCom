/*******************************************************************************
 * Variables.h
 *
 * Class definitions, structs and typedefs.
 *******************************************************************************
 * Date of creation: 10.11.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 ******************************************************************************/

#ifndef VARIABLES_H_
#define VARIABLES_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <mutex>
#include <iostream>
#include <vector>
#include "Helpers.h"

using namespace std;
/*******************************************************************************
 * Defines
 ******************************************************************************/
#define BYTE_LENGTH_DESCRIPTION 32
/*******************************************************************************
 * Typedefs
 ******************************************************************************/
// Device supported variable types
typedef enum
{
	TYPE_UI8	= 0,
	TYPE_I8		= 1,
	TYPE_UI16	= 2,
	TYPE_I16	= 3,
	TYPE_UI32	= 4,
	TYPE_I32	= 5,
	TYPE_F32	= 6
}VAR_TYPE;

typedef enum
{
	RO	= 0,
	RW	= 1
}ACCESS;

typedef unsigned int EEPROMVarNr;
typedef unsigned int RAMVarNr;

// Structure of a device supported EEPROM variable
typedef struct
{
	short			EEPROMVarNr;
	VAR_TYPE		EEPROMVarType;
	double			EEPROMVarValue;
	double			EEPROMVarDefault;
	double			EEPROMVarMinVal;
	double			EEPROMVarMaxVal;
	char			description[BYTE_LENGTH_DESCRIPTION];
	ACCESS			access;
}EEPROMVAR;

// Structure of a devie supported RAM variable
typedef struct
{
	short			RAMVarNr;
	VAR_TYPE		RAMVarType;
	double			RAMVarValue;
	double			RAMVarMinVal;
	double			RAMVarMaxVal;
	char			description[BYTE_LENGTH_DESCRIPTION];
	ACCESS			access;
}RAMVAR;

typedef struct
{
	bool 		initialized;
	EEPROMVAR	variable;
}EEPROMVARMEMBER;

typedef struct
{
	bool 		initialized;
	RAMVAR	variable;
}RAMVARMEMBER;
/*******************************************************************************
 * Class declarations
 ******************************************************************************/
class EEPROMVariables
{

public:
	// Variable numbers
	unsigned int m_numberOfVars;
	// The actual Variable lists
	vector<EEPROMVARMEMBER>*	m_EEPROMVars = nullptr;

	// Public member functions
	EEPROMVariables(int noOfVars);
	~EEPROMVariables(void);

	// �berladung des Indexoperators (Zugriff auf Variablenwert durch inizierung)
	EEPROMVAR& operator[](int i)
	{
		return (*m_EEPROMVars)[i].variable;
	}

	// EEPROM and RAMVariable Initializer
	int InitVarByNumber (EEPROMVAR varInitializer);
	EEPROMVAR GetVarByNumber(unsigned short varNr);
	int SetVarByNumber(unsigned short varNr, double value);

private:
	// HelperFunctions *	m_helPtr;
	// This class has to be thread save
	mutex m_Accessmutex;
};

class RAMVariables
{

public:
	// Variable numbers
	unsigned int m_numberOfVars;
	// The actual Variable lists
	vector<RAMVARMEMBER>*	m_RAMVars = nullptr;

	// Public member functions
	RAMVariables(int noOfVars);
	~RAMVariables(void);

	// �berladung des Indexoperators (Zugriff auf Variable durch indizierung)
	RAMVAR& operator[](int i)
	{
		return (*m_RAMVars)[i].variable;
	}

	// EEPROM and RAMVariable Initializer
	int InitVarByNumber (RAMVAR varInitializer);
	RAMVAR GetVarByNumber(unsigned short varNr);
	int SetVarByNumber(unsigned short varNr, double value);

private:
	// HelperFunctions *	m_helPtr;
	// This class has to be thread save
	mutex m_Accessmutex;
};
#endif /* VARIABLES_H_ */
