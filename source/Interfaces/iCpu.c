/*
 * iCpu.c
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#include "iCpu.h"
#include <MKL46Z4.h>

// Définition des adresses des registres MCG
#define kMCG_C1	0x40064000
#define kMCG_C2	0x40064001
#define kMCG_C5	0x40064004
#define kMCG_C6	0x40064005
#define kMCG_S	0x40064006

//------------------------------------------------------------
// Core and peripheral clock init
// External clock = 8MHz
// Core clock = 48MHz
// Peripheral bus = 24MHz
// Flexbus = 24MHz
// Flash = 24MHz
//------------------------------------------------------------
void iCpu_CLKInit(void)
{
	unsigned char *aPtr;
	
	// 1.	Configuration du type de quartz utilisé, 
	// de l’échelle de sa fréquence et du type de connexion du quartz externe de 8 MHz dans le registre MCG Control Register 2
	aPtr=(unsigned char *)kMCG_C2;
	*aPtr&= 0xC3;		// Reset des champs
	*aPtr|= 0x1C;		// Range0=01, HGO0=1 et EREFS0=1
	
	// Sélection de la source du clock, division de la fréquence de la source du clock, 
	// attention pour le passage en mode FBE (FLL Bypass External Clock)  
	aPtr=(unsigned char *)kMCG_C1;
	*aPtr&= 0x03;		// Reset des champs
	*aPtr|= 0x98;		// CLKS=10, FRDIV=011 et IREFS=0
	
	// 3.	Attendre de l’oscillateur externe soit initialiser, que le clock source de la FLL soit le clock externe et 
	// que c’est bien l’horloge externe comme source du clock MCGOUTCLK (le clock source CPU et BUS), 
	// c’est-à-dire que l’on est passé en mode FBE
	aPtr=(unsigned char *)kMCG_S;
	while((*aPtr&0x02)==0); 		// Wait OSCINIT0 = 1
	while((*aPtr&0x10)==0x10); 	// Wait IREFST = 0
	while((*aPtr&0x08)!=0x08); 	// Wait CLKST = 01
	
	// 4.	Configuration d’une fréquence de référence correcte pour la PLL (entre 2 et 4MHz)
	aPtr=(unsigned char *)kMCG_C5;
	*aPtr&= 0xE0;		// Reset des champs
	*aPtr|= 0x03;		// PRDIV0=00011
	
	// 5.	Passage en mode PBE (PLL Bypass External Clock)
	aPtr=(unsigned char *)kMCG_C6;
	*aPtr&= 0xA0;		// Reset des champs
	*aPtr|= 0x40;		// PLLS=1 et VDIV0=0
	
	// 6.	Attendre que la source du clock CPU soit la PLL et attendre que le clock PLL soit stable.
	aPtr=(unsigned char *)kMCG_S;
	while((*aPtr&0x20)==0); 		// Wait PLLST = 1
	while((*aPtr&0x40)==0x0); 	// Wait LOCK0 = 1
	
	// 7.	Transition en mode PEE
	aPtr=(unsigned char *)kMCG_C1;
	*aPtr&= 0x3F;		// CLKS=0

	// 8.	Attendre que la PLL soit la source du clock CPU
	aPtr=(unsigned char *)kMCG_S;
	while((*aPtr&0x0C)!=0x0C); 		// Wait CLKST = 11
}

