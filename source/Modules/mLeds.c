/*
 * mLeds.c
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#include "mLeds.h"
#include "iDio.h"


//-----------------------------------------------------------------------------
// Configuration des ports IO utilisés par les LEDS
//-----------------------------------------------------------------------------
void mLeds_Setup(void)
{
		// Enable du clock des GPIO
		iDio_EnablePortClk();
		
		// Configuration de la fonctionnalité de chaque PIN
		iDio_PinConfig(kPortA,kPin4,kAlternate1);
		iDio_PinConfig(kPortA,kPin5,kAlternate1);
		iDio_PinConfig(kPortA,kPin12,kAlternate1);
		iDio_PinConfig(kPortA,kPin13,kAlternate1);
		iDio_PinConfig(kPortC,kPin8,kAlternate1);
		iDio_PinConfig(kPortC,kPin9,kAlternate1);
		iDio_PinConfig(kPortD,kPin2,kAlternate1);
		iDio_PinConfig(kPortD,kPin3,kAlternate1);
		
		
		// Configuration des Ports en entrée ou en sortie
		iDio_SetPortDirection(kPortA,kMaskIo4+kMaskIo5+kMaskIo12+kMaskIo13,kIoOutput);
	  iDio_SetPortDirection(kPortC,kMaskIo8+kMaskIo9,kIoOutput);
	  iDio_SetPortDirection(kPortD,kMaskIo2+kMaskIo3,kIoOutput);
}

//-----------------------------------------------------------------------------
// On Off des LED
// aMask:   choix des LED sur lesquelles on veut agir (si bit à 1)
// aState:  choix de l'état des LED désirés
//-----------------------------------------------------------------------------
void mLeds_Write(LedMaskEnum aMask,LedStateEnum aState)
{  
  unsigned long aTmp;
    
	// Commande des LED --> PORTA, PORTC et PORTD
  // Correspondance entre le masque des LEDS et le masque des PORTS
  // Port: 15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0	
  // Leds:														8	7	6	5	4	3	2	1
  if(kLedOn==aState)
    {
      // Leds	--> portA
  		// 2				13
  		// 5        5
  		// 6        4
  		// 7				12
  		aTmp=(0|((aMask&0x2)<<12)|((aMask&0x10)<<(1))|((aMask&0x20)>>(1))|((aMask&0x40)<<(6)));
  		iDio_SetPort(kPortA,(IoMaskEnum)(aTmp),kIoOn);
  		
  		// Leds	--> portC
			// 3				9
			// 4        8
			aTmp=(0|((aMask&0x4)<<7)|((aMask&0x8)<<(5)));
			iDio_SetPort(kPortC,(IoMaskEnum)(aTmp),kIoOn);
  		
			// Leds	--> portD
			// 1				2
			// 8        3
			aTmp=(0|((aMask&0x1)<<2)|((aMask&0x80)>>(4)));
			iDio_SetPort(kPortD,(IoMaskEnum)(aTmp),kIoOn);
    }
  else if(kLedOff==aState)
    {
  		// Leds	--> portA
			// 2				13
			// 5        5
			// 6        4
			// 7				12
			aTmp=(0|((aMask&0x2)<<12)|((aMask&0x10)<<(1))|((aMask&0x20)>>(1))|((aMask&0x40)<<(6)));
			iDio_SetPort(kPortA,(IoMaskEnum)(aTmp),kIoOff);
			
			// Leds	--> portC
			// 3				9
			// 4        8
			aTmp=(0|((aMask&0x4)<<7)|((aMask&0x8)<<(5)));
			iDio_SetPort(kPortC,(IoMaskEnum)(aTmp),kIoOff);
			
			// Leds	--> portD
			// 1				2
			// 8        3
			aTmp=(0|((aMask&0x1)<<2)|((aMask&0x80)>>(4)));
			iDio_SetPort(kPortD,(IoMaskEnum)(aTmp),kIoOff);
    }
}
