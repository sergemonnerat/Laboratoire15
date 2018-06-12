/*
------------------------------------------------------------
Copyright 2003-20xx Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	iUart.c
Auteur et Date :	Monnerat Serge 11.01.20xx

Description dans le fichier iUart.h
------------------------------------------------------------
*/

#include "iUart.h"
#include <MKL46Z4.h>
#include <core_cm0plus.h>

// Fr�quence du bus
#define kBusClockkHz 24000

// Vitesse de transmission
#define kUart2BaudRate 115200

// Priorit� de l'interruption RX
#define kUart2Prio 1

// 50 bytes buffer
#define kRxBufSize ((UInt8)(50)) 

// RX buffer control struct
static struct
{
	Int8     	RxBuf[kRxBufSize];
	UInt16 	  InIndex;
	UInt16 	  OutIndex;
	UInt16    ByteCount;
	bool      BufferIsFull;
}sUartRxBufStruct;

//-----------------------------------------------------------------------
// UART 2 module configuration
//-----------------------------------------------------------------------
void iUart_Config(void)
{
	unsigned int aSbr;
	
	// Enable du clock de l'UART2
	// System Clock Gating Control Register 4 (SIM_SCGC4)
	SIM->SCGC4|=SIM_SCGC4_UART2_MASK;
	
	// Configuration du crossbar pour les pin RX et TX (PTE16 et PTE17)
	// Pin Control Register n (PORTx_PCRn) --> UART = alternative 3
	PORTE->PCR[16]|=PORT_PCR_MUX(3);
	PORTE->PCR[17]|=PORT_PCR_MUX(3);
	
	// UART Control Register 1 (UARTx_C1)
	// PE = 0, pas de parit�
	// M = 0 Normal --> start + 8 data bits (lsb first) + stop
	UART2->C1&= ~(UART_C1_M_MASK|UART_C1_PE_MASK );
	
	// Configuration de la vitesse de transmission
	// UART Baud Rate Registers:High (UARTx_BDH)
	// UART Baud Rate Registers: Low (UARTx_BDL)
	aSbr = (unsigned int)((kBusClockkHz*1000)/(kUart2BaudRate * 16));		
	UART2->BDH&=(~UART2->BDH);
	UART2->BDH|=UART_BDH_SBR(((aSbr & 0x1F00) >> 8));
	UART2->BDL= (unsigned char)(aSbr & UART_BDL_SBR_MASK);
	
	
	// Enable de l'interruption UART2 Rx au niveau du NVIC
	// Le vecteur du UART2 est le num�ro 14
	NVIC_EnableIRQ(UART2_IRQn);
		
	// Configuration de la priorit� de l'interruption PIT
	// O : plus haute priorit�
	// 3 : plus basse priorit�
	NVIC_SetPriority(UART2_IRQn, kUart2Prio);
	
	// Disable des interruptions
	// UART Control Register 2 (UARTx_C2)
	// UART Control Register 3 (UARTx_C3)
	UART2->C2&= (~(UART_C2_TIE_MASK|UART_C2_TCIE_MASK|UART_C2_RIE_MASK|UART_C2_ILIE_MASK));
	UART2->C3&= (~(UART_C3_ORIE_MASK|UART_C3_NEIE_MASK|UART_C3_FEIE_MASK|UART_C3_PEIE_MASK));
	
	// Enable de l'interruption RX
	// UART Control Register 2 (UARTx_C2)
	UART2->C2|=UART_C2_RIE_MASK;
}

//------------------------------------------------------------
// Enable/Disable de la transmission --> TX
//------------------------------------------------------------
void iUart_EnDisTx(UartEnDisEnum aEnDis)
{
	// UART Control Register 2 (UARTx_C2)
	if(kEn==aEnDis)
		{
			UART2->C2|=UART_C2_TE_MASK; 		// Enable TX
		}
	else if(kDis==aEnDis)
		{
			UART2->C2&=(~UART_C2_TE_MASK); // Disable TX
		}
}


//------------------------------------------------------------
// Enable/Disable de la r�ception
//------------------------------------------------------------
void iUart_EnDisRx(UartEnDisEnum aEnDis)
{
	// UART Control Register 2 (UARTx_C2)
	if(kEn==aEnDis)
		{
			UART2->C2|=UART_C2_RE_MASK; 		// Enable RX
		}
	else if(kDis==aEnDis)
		{
			UART2->C2&=(~UART_C2_RE_MASK); // Disable RX
		}
}

//------------------------------------------------------------
// Get Uart2 flags state
// aStatus: which flag to read
// retour	: flag state
//------------------------------------------------------------
bool iUart_GetStatus(UartStatusEnum aStatus)
{
	// UART Status Register 1 (UARTx_S1)
	return ((UART2->S1&aStatus)==aStatus);
}

//------------------------------------------------------------
// Uart2 Data register write
// aData: datas to transmit
//------------------------------------------------------------
void iUart_SetData(unsigned char aData)
{
	// UART Data Register (UARTx_D)
	UART2->D=aData;
}

//------------------------------------------------------------
// Initialisation du buffer de r�ception
// !!! Il faut que l'interrupt soit disable --> probl�me de 
// concurence !!!
// aVal: la valeur avec laquelle on initialise le buffer 
//------------------------------------------------------------
void iUart_InitRxBuffer(unsigned char aVal)

{
	UInt16 i=0;
	
	
	// Initialisation des index
	sUartRxBufStruct.InIndex=0;
	sUartRxBufStruct.OutIndex=0;
	
	// Reset du compteur de bytes
	sUartRxBufStruct.ByteCount=0;
	
	// Reset du flag indiquant que le buffer est plein
	sUartRxBufStruct.BufferIsFull=false;
	
	// Reset du buffer
	for(i=0;i<kRxBufSize;i++)
		{
			sUartRxBufStruct.RxBuf[i]=aVal; 
		}
	
}

//------------------------------------------------------------
// Contr�le si le buffer est vide
// Return : true --> buffer empty, false --> buffer not empty
//------------------------------------------------------------
bool iUart_IsBufferEmpty(void)
{
	bool aRet=false;
	
	if(sUartRxBufStruct.ByteCount==0)
		{
			aRet=true;
		}
	else
		{
			aRet=false;
		}

	return aRet;
}

//------------------------------------------------------------
// Lecture d'un byte dans le buffer
// Return : retourne le plus vieux byte
//------------------------------------------------------------
Int8 iUart_GetCharFromBuffer(void)
{
  Int8 aChar=0;
	
	// Byte read
	aChar=sUartRxBufStruct.RxBuf[sUartRxBufStruct.OutIndex];
	
	// Incr�mentation de l'index
	sUartRxBufStruct.OutIndex++;
	
	// Buffer tournant
	if(sUartRxBufStruct.OutIndex>=kRxBufSize)
		{
			sUartRxBufStruct.OutIndex=0;
		}
	
	// D�cr�mentation du compteur de bytes
	sUartRxBufStruct.ByteCount--;
	 
	// Retourne un byte du buffer
	return aChar;
}

//---------------------------------------------------------------------------
// RX interrupt 
//---------------------------------------------------------------------------
void UART2_IRQHandler(void)
{
	Int8 aVal;
	
	// Lecture de la donn�e re�ue et du registre de statut (clear du flag)
	UART2->S1;
	aVal=UART2->D;
	
	
	if(sUartRxBufStruct.ByteCount>=kRxBufSize)
	  {
	    // Buffer full flag
	    sUartRxBufStruct.BufferIsFull=true;
	  }
	else
	  {
	    // Buffer not full flag
	    sUartRxBufStruct.BufferIsFull=false;
	    
	    // Sauvegarde de la donn�e re�ue
	    sUartRxBufStruct.RxBuf[sUartRxBufStruct.InIndex]=aVal;
	    
	    // Incr�mentation de l'index
	    sUartRxBufStruct.InIndex++;
	    
	    // Incr�mentation du compteur de byte
	    sUartRxBufStruct.ByteCount++;
	
	    // Buffer tournant
	    if(sUartRxBufStruct.InIndex>=kRxBufSize)
	      {
	        sUartRxBufStruct.InIndex=0;
	      }
	  }
}


