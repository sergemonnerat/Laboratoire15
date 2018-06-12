/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	iI2C.c
Auteur et Date :	Monnerat Serge 8.5.20xx

Description dans le fichier iI2C.h
------------------------------------------------------------
*/

#include "iI2C.h"
#include <MKL46Z4.h>

//------------------------------------------------------------
// Configuration du module I2C
//------------------------------------------------------------
void iI2C_Config(void)
{
	
	// I2C clock enable
	// System Clock Gating Control Register 4 (SIM_SCGC4)
	SIM->SCGC4|=SIM_SCGC4_I2C1_MASK;
	
	// Port C I2C pin setup for I2C (alternate 2)
	// Pin Control Register n (PORTx_PCRn)
	PORTC->PCR[1]&=(~PORT_PCR_MUX_MASK);
	PORTC->PCR[1]|=PORT_PCR_MUX(2);
	PORTC->PCR[2]&=(~PORT_PCR_MUX_MASK);
	PORTC->PCR[2]|=PORT_PCR_MUX(2);
	
	
	// Baud rate speed and I2C timing
	// I2C Frequency Divider register (I2Cx_F)
	// I2C clock rate=375 kHz (max 400kHz)
	// SDA Hold = 0.54us (max 0.9us)
	// SCL start Hold = 1.08us (min 0.6us)
	// SCL stop Hold = 1.38 us (min 0.6us
	I2C1->F=0;
	I2C1->F|=I2C_F_ICR(0x12)|I2C_F_MULT(0);
}


//------------------------------------------------------------
// I2C interface enable
//------------------------------------------------------------
void iI2C_Enable(void)
{

	// I2C0 enable
	// I2C Control Register 1 (I2Cx_C1)
	I2C1->C1 |= I2C_C1_IICEN_MASK;
}

//------------------------------------------------------------
// I2C interface disable
//------------------------------------------------------------
void iI2C_Disable(void)
{
	// I2C0 disable
	// I2C Control Register 1 (I2Cx_C1)
	I2C1->C1 &= (~I2C_C1_IICEN_MASK);
}

//------------------------------------------------------------
// Set START state
//------------------------------------------------------------
void iI2C_SetStartState(void)
{
	// I2C Control Register 1 (I2Cx_C1)
	I2C1->C1 |= I2C_C1_MST_MASK;
}

//------------------------------------------------------------
// Set repeated START state
//------------------------------------------------------------
void iI2C_SetRepeatedStartSate(void)
{
	// I2C Control Register 1 (I2Cx_C1)
	I2C1->C1 |= I2C_C1_RSTA_MASK;
}

//------------------------------------------------------------
// Set STOP state
//------------------------------------------------------------
void iI2C_SetStopState(void)
{
	// I2C Control Register 1 (I2Cx_C1)
	I2C1->C1 &= (~I2C_C1_MST_MASK);
}

//------------------------------------------------------------
// Generate automatic ACK or not
//------------------------------------------------------------
void iI2C_SetAckMode(I2CAckEnum aAck)
{

	if (aAck == kAckAuto)
		{
			// I2C Control Register 1 (I2Cx_C1)
			I2C1->C1 &= (~I2C_C1_TXAK_MASK);
		}
	else if (aAck == kNoAck)
		{
			// I2C Control Register 1 (I2Cx_C1)
			I2C1->C1 |= I2C_C1_TXAK_MASK;
		}
}

//------------------------------------------------------------
// Select if we transmit or receive
//------------------------------------------------------------
void iI2C_TxRxSelect(I2CTransmiteModeEnum aMode)
{
	if (kTxMode == aMode)
		{
			// TX
			I2C1->C1 |= I2C_C1_TX_MASK;
		}
	else if (kRxMode == aMode)
		{
			// RX
			I2C1->C1 &= ~I2C_C1_TX_MASK;
		}
}

//------------------------------------------------------------
// Send a data
//------------------------------------------------------------
void iI2C_SendData(UInt8 aData)
{
	// I2C Data I/O register (I2Cx_D)
	I2C1->D = aData;
}

//------------------------------------------------------------
// Wait End of transmit or receive
//------------------------------------------------------------
void iI2C_WaitEndOfRxOrTx(void)
{
	// Wait for IICIF flag
	// I2C Status register (I2Cx_S)
	while ((I2C1->S & I2C_S_IICIF_MASK) == 0);
	// Clear the IICIF flag
	I2C1->S |= I2C_S_IICIF_MASK;
}

//------------------------------------------------------------
// Read received data, ! generate I2C clock if not in STOP mode
//------------------------------------------------------------
UInt8 iI2C_ReadData(void)
{
	UInt8 aReturnData = 0;
	
	// I2C Data I/O register (I2Cx_D)
	aReturnData = I2C1->D;
	
	return aReturnData;
}

//------------------------------------------------------------
// Get I2C status flags
//------------------------------------------------------------
bool iI2C_ReadStatus(I2CStatusEnum aStatus)
{
	return (bool)(((I2C1->S&aStatus)==aStatus));
}
