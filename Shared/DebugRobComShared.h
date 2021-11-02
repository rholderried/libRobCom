/********************************************************************************//**
 * \file DebugRobComShared.h
 * \author Roman Holderried
 *
 * \brief Header file just for debugging the interface. 
 *
 * <b> History </b>
 *      - 2021-09-30 - File creation. 
 *                     
 ***********************************************************************************/

#ifndef DEBUGROBCOMSHARED_H_
#define DEBUGROBCOMSHARED_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "RobCom.h"

/*******************************************************************************
 * Function definitions
 ******************************************************************************/
RobCom* RobComInit(void);
void debugMsgInterface(RobCom* pInst, unsigned int portNo, unsigned int baudrate);
uint32_t getDebugMsg(RobCom* pInst, uint8_t** buffer);
// uint8_t* getDebugMsg(RobCom* pInst);
void deleteRobCom(RobCom* pInst);


#endif /* DEBUGROBCOMSHARED_H_ */