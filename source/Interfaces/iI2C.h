/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	iI2C.h	
Auteur et Date :	Monnerat Serge 8.5.20xx

But : Interface du périphérique I2C 

Modifications
Date		Faite	Ctrl		Description
------------------------------------------------------------
*/
#ifndef __I2C__
#define __I2C__

#include "def.h"


typedef enum
{
	kAckAuto,
	kNoAck
}I2CAckEnum;

typedef enum
{
	kTransferComplete=0x80,
	kInterruptFlag=0x2,
	kRxAK=0x1,
	kBUSY=0x20
}I2CStatusEnum;

typedef enum
{
	kTxMode,
	kRxMode
}I2CTransmiteModeEnum;

//------------------------------------------------------------
// I2C module config
//------------------------------------------------------------
void iI2C_Config(void);

//------------------------------------------------------------
// I2C interface enable
//------------------------------------------------------------
void iI2C_Enable(void);

//------------------------------------------------------------
// I2C interface disable
//------------------------------------------------------------
void iI2C_Disable(void);

//------------------------------------------------------------
// Set START state
//------------------------------------------------------------
void iI2C_SetStartState(void);

//------------------------------------------------------------
// Set repeated START state
//------------------------------------------------------------
void iI2C_SetRepeatedStartSate(void);

//------------------------------------------------------------
// Set STOP state
//------------------------------------------------------------
void iI2C_SetStopState(void);

//------------------------------------------------------------
// Generate automatic ACK or not
//------------------------------------------------------------
void iI2C_SetAckMode(I2CAckEnum aAck);

//------------------------------------------------------------
// Select if we transmit or receive
//------------------------------------------------------------
void iI2C_TxRxSelect(I2CTransmiteModeEnum aMode);

//------------------------------------------------------------
// Send a data
//------------------------------------------------------------
void iI2C_SendData(UInt8 aData);

//------------------------------------------------------------
// Wait End of transmit or receive
//------------------------------------------------------------
void iI2C_WaitEndOfRxOrTx(void);

//------------------------------------------------------------
// Read received data, ! generate I2C clock if not in STOP mode
//------------------------------------------------------------
UInt8 iI2C_ReadData(void);

//------------------------------------------------------------
// Get I2C status flags
//------------------------------------------------------------
bool iI2C_ReadStatus(I2CStatusEnum aStatus);

#endif
