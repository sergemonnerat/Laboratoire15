/*
 * iDio.c
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#include "iDio.h"
#include <MKL46Z4.h> /* include peripheral declarations */

//-------------------------------------------------------------------------
// D�claration des constantes
//-------------------------------------------------------------------------
// Registres de direction
#define kGPIOA_PDDR     0x400FF014
#define kGPIOB_PDDR     0x400FF054
#define kGPIOC_PDDR     0x400FF094
#define kGPIOD_PDDR     0x400FF0D4
#define kGPIOE_PDDR     0x400FF114

// Registre de donn�es --> sortie
#define kGPIOA_PDOR     0x400FF000
#define kGPIOB_PDOR     0x400FF040
#define kGPIOC_PDOR     0x400FF080
#define kGPIOD_PDOR     0x400FF0C0
#define kGPIOE_PDOR     0x400FF100

// Registre de donn�es --> entr�e
#define kGPIOA_PDIR     0x400FF010
#define kGPIOB_PDIR     0x400FF050
#define kGPIOC_PDIR     0x400FF090
#define kGPIOD_PDIR     0x400FF0D0
#define kGPIOE_PDIR     0x400FF110

// Registre permettant de configurer la fonctionnalit� des pins
#define kPORTA_PCR0			0x40049000
#define kPORTB_PCR0			0x4004A000
#define kPORTC_PCR0			0x4004B000
#define kPORTD_PCR0			0x4004C000
#define kPORTE_PCR0			0x4004D000

//-----------------------------------------------------------------------------
// Enable du clock des GPIO
//-----------------------------------------------------------------------------
void iDio_EnablePortClk(void)
{
		// Pointeur permettant d'acc�der aux registres d'enable des clocks des port IO
	unsigned long *aSIMSCGC5Addr=(unsigned long*)0x40048038;	
	
	// ------------------------------------------------------------
	// Enable du clock des ports IO
	// ------------------------------------------------------------	
	*aSIMSCGC5Addr=(0|(1<<9)|(1<<10)|(1<<11)|(1<<12)|(1<<13));
}

//-----------------------------------------------------------------------------
// Configuration de la fonctionnalit� de chaque PIN
//-----------------------------------------------------------------------------
void iDio_PinConfig(PortIOEnum aPort,PinNbEnum aPinNb, PinFunctionEnum aPinFunc)
{
	unsigned long *aPortPtr;
	
	// Configuration du pointeur selon le port pass� en param�tre
	switch(aPort)
		{
			// DDRA
			case kPortA:
				aPortPtr=(unsigned long *)(kPORTA_PCR0+(aPinNb*4));
				
			break;
			
			// DDRB
			case kPortB:
				aPortPtr=(unsigned long *)(kPORTB_PCR0+(aPinNb*4));
			break;
			
			// DDRC
			case kPortC:
				aPortPtr=(unsigned long *)(kPORTC_PCR0+(aPinNb*4));
			break;
			
			// DDRD
			case kPortD:
				aPortPtr=(unsigned long *)(kPORTD_PCR0+(aPinNb*4));
			break;
			
			// DDRE
			 case kPortE:
				 aPortPtr=(unsigned long *)(kPORTE_PCR0+(aPinNb*4));
			 break;
		}     
	
	// Reset du champ MUX
	*aPortPtr&=0xFFFFF8FF;
	// Configuration de la fonction
	*aPortPtr|=aPinFunc;
}

//-----------------------------------------------------------------------------
// Configuration des Ports en entr�e ou en sortie
// aPort: le port IO que l'on veut configurer
// aMask: choix des pins du port sur lesquelles on veut agir (si bit � 1)
// aDir:  choix de la direction des pins s�lectionn�es avec aMask
//-----------------------------------------------------------------------------
void iDio_SetPortDirection(PortIOEnum aPort,IoMaskEnum aMask,IoDirectionEnum aDir)
{
  unsigned long *aPortPtr;
  
  
  // Configuration du pointeur selon le port pass� en param�tre
  switch(aPort)
    {
      // DDRA
      case kPortA:
        aPortPtr=(unsigned long *)(kGPIOA_PDDR);
      break;
      
      // DDRB
      case kPortB:
        aPortPtr=(unsigned long *)(kGPIOB_PDDR);
      break;
      
      // DDRC
      case kPortC:
        aPortPtr=(unsigned long *)(kGPIOC_PDDR);
      break;
      
      // DDRD
      case kPortD:
        aPortPtr=(unsigned long *)(kGPIOD_PDDR);
      break;
      
      // DDRE
       case kPortE:
         aPortPtr=(unsigned long *)(kGPIOE_PDDR);
       break;
    } 
    
  // Configuration des bits du port d�sir� en entr�e ou sortie
  if(kIoOutput==aDir)
    {
      *aPortPtr|=aMask;
    }
  else if(kIoInput==aDir)
    {
      *aPortPtr &=~aMask;
    } 
}


//-----------------------------------------------------------------------------
// Affectation de l'�tat des pins du port configur�es en sortie
// aMask:   choix des pins du port sur lesquelles on veut agir (si bit � 1)
// aState:  choix de l'�tat des bits du port d�sir�s
//-----------------------------------------------------------------------------
void iDio_SetPort(PortIOEnum aPort,IoMaskEnum aMask,IoStateEnum aState)
{
  unsigned long *aPortPtr;
  
  // Configuration du pointeur selon le port pass� en param�tre
  switch(aPort)
    {
      // DDRA
      case kPortA:
        aPortPtr=(unsigned long *)(kGPIOA_PDOR);
      break;
      
      // DDRB
      case kPortB:
        aPortPtr=(unsigned long *)(kGPIOB_PDOR);
      break;
      
      // DDRC
      case kPortC:
        aPortPtr=(unsigned long *)(kGPIOC_PDOR);
      break;
      
      // DDRD
      case kPortD:
        aPortPtr=(unsigned long *)(kGPIOD_PDOR);
      break;
      
      // DDRE
       case kPortE:
         aPortPtr=(unsigned long *)(kGPIOE_PDOR);
       break;
    } 
    
  // Configuration de l'�tat des bits du port configur�s en sortie
  if(kIoOn==aState)
    {
      *aPortPtr|=aMask;
    }
  else if(kIoOff==aState)
    {
      *aPortPtr&=~aMask;
    }
}

//-----------------------------------------------------------------------------
// Lecture de l'�tat des pins du port configur�es en entr�e
// aMask:   de quelle pin ou veut l'�tat 
// retour:  �tat de la pin --> true ou false
//-----------------------------------------------------------------------------
bool iDio_GetPort(PortIOEnum aPort,IoMaskEnum aMask)
{
  bool  aRet=false;
  unsigned long *aPortPtr;
  
  // Configuration du pointeur selon le port pass� en param�tre
  switch(aPort)
    {
      // DDRA
      case kPortA:
        aPortPtr=(unsigned long *)(kGPIOA_PDIR);
      break;
      
      // DDRB
      case kPortB:
        aPortPtr=(unsigned long *)(kGPIOB_PDIR);
      break;
      
      // DDRC
      case kPortC:
        aPortPtr=(unsigned long *)(kGPIOC_PDIR);
      break;
      
      // DDRD
      case kPortD:
        aPortPtr=(unsigned long *)(kGPIOD_PDIR);
      break;
      
      // DDRE
       case kPortE:
         aPortPtr=(unsigned long *)(kGPIOE_PDIR);
       break;
    }     
 
  // Lecture de l'�tat des bit du port configur�s en entr�e
  aRet=(bool)((*aPortPtr&aMask)==aMask);
  
  return aRet;
}

//-----------------------------------------------------------------------------
// Le lien entre la donn�e ou la commande � transmettre et les bits du port E et B
// aVal:   la commande ou la donn�e
//-----------------------------------------------------------------------------
void iDio_PortLcd(unsigned char aVal)
{
	// Bit0,	Bit1,		Bit2,		Bit3,		Bit4,		Bit5,		Bit6,		Bit7,		
	// PTB20,	PTE21, 	PTE20,	PTE2,		PTE3,	  PTE6,	  PTE18,	PTE19,	
	
	// Reset des bits du port E utilis�s par le LCD
	GPIOE->PDOR&=(~(0|((1<<2)|(1<<3)|(1<<6)|(1<<18)|(1<<19)|(1<<20)|(1<<21))));
	
	// Reset des bits du port B utilis�s par le LCD
	GPIOB->PDOR&=(~(0|((1<<20))));
	
	// Set des bits du port E utilis�s par le LCD
	// Example: Data1 --> bit 21 du port E --> d�placement de 21-1 sur la gauche
	// Example: Data6 --> bit 18 du port E --> d�placement de 18-6 sur la gauche
	GPIOE->PDOR|=(((aVal&0x02)<<20)|((aVal&0x04)<<18)|((aVal&0x08)>>1)|((aVal&0x10)>>1)|((aVal&0x20)<<1)|((aVal&0x40)<<12)|((aVal&0x80)<<12));
	
	// Set des bits du port B utilis�s par le LCD
	GPIOB->PDOR|=(((aVal&0x01)<<20));
}
