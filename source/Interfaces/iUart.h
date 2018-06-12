/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name : 	iUart.h	
Author and date :	Monnerat Serge 06.06.2014

Goal : interface du périphérique RS232

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#ifndef __iUart__
#define __iUart__

#include "def.h"

//------------------------------------------------------------
// Enum contenant le choix entre ENABLE et DISABLE de TX/RX
//------------------------------------------------------------
typedef enum
{
  kEn,
  kDis
}UartEnDisEnum;

//------------------------------------------------------------
// UART status flag
//------------------------------------------------------------
typedef enum
{
	kTransmitDataRegEmpty=0x80,
	kTransmitComplete=0x40,
	kReceiveDataRegFull=0x20,
	kIdleLineDetect=0x10,
	kOverrunErr=0x08,
	kNoiseErr=0x04,
	kFramingErr=0x02,
	kParityErr=0x01
}UartStatusEnum;

//-----------------------------------------------------------------------
// UART 2 module configuration
//-----------------------------------------------------------------------
void iUart_Config(void);

//------------------------------------------------------------
// Enable/Disable de la transmission --> TX
//------------------------------------------------------------
void iUart_EnDisTx(UartEnDisEnum aEnDis);

//------------------------------------------------------------
// Enable/Disable de la réception
//------------------------------------------------------------
void iUart_EnDisRx(UartEnDisEnum aEnDis);

//------------------------------------------------------------
// Get Uart2 flags state
// aStatus: which flag to read
// retour	: flag state
//------------------------------------------------------------
bool iUart_GetStatus(UartStatusEnum aStatus);

//------------------------------------------------------------
// Uart2 Data register write
// aData: datas to transmit
//------------------------------------------------------------
void iUart_SetData(unsigned char aData);

//------------------------------------------------------------
// Initialisation du buffer de réception
// !!! Il faut que l'interrupt soit disable --> problème de 
// concurence !!!
// aVal: la valeur avec laquelle on initialise le buffer 
//------------------------------------------------------------
void iUart_InitRxBuffer(unsigned char aVal);

//------------------------------------------------------------
// Contrôle si le buffer est vide
// Return : true --> buffer empty, false --> buffer not empty
//------------------------------------------------------------
bool iUart_IsBufferEmpty(void);

//------------------------------------------------------------
// Lecture d'un byte dans le buffer
// Return : retourne le plus vieux byte
//------------------------------------------------------------
Int8 iUart_GetCharFromBuffer(void);

#endif
