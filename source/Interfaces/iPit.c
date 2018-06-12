/*
 * iPit.c
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#include "iPit.h"
#include <MKL46Z4.h>

//------------------------------------------------------------
// D�claration des constantes
//------------------------------------------------------------
// Core clock
#define kClockCpu 48
#define kClockBus 24

// Priorit� de l'interruption PIT
#define kPITPrio 4

// Adresse des registres
#define kPIT_MCR 			0x40037000
#define kPIT_LDVAL0 	0x40037100
#define kPIT_LDVAL1 	0x40037110
#define kPIT_TCTRL0		0x40037108
#define kPIT_TCTRL1		0x40037118
#define kPIT_TFLG0		0x4003710C
#define kPIT_TFLG1		0x4003711C
#define kNVIC_ISER		0xE000E100
#define kNVIC_IPR5		0xE000E414
#define kSIM_SCGC6    0x4004803C

#define kNbOfDelay 20

//------------------------------------------------------------
// D�claration des var. globales
//------------------------------------------------------------
static bool sPitFlag=false;

struct DelayStruct
{
	unsigned int Counter;		// La valeur du delay d�sir�	
	bool IsFree ;					// Est-ce que le delay est d�j� utilis�?
	bool IsDone ;					// Est-ce que le delay est �chu?
}; 

static struct DelayStruct sDelayTab[kNbOfDelay];

//------------------------------------------------------------
// Configuration du PIT
// aPIT0PeriodeMs  	: p�riode en ms du PIT0
// aPIT1PeriodeMs  	: p�riode en ms du PIT1
//------------------------------------------------------------
void iPit_Config(unsigned int aPIT0PeriodeMs,unsigned int aPIT1PeriodeMs)
{
	static unsigned long aTmp=0;
	unsigned long *aPtr;
	
	// System Clock Gating Control Register 6 (SIM_SCGC6)
	// Enable du clock pour le p�riph�rique PIT
	aPtr=(unsigned long*)kSIM_SCGC6;
	*aPtr|=(1<<23);
	
	// PIT Module Control Register (PIT_MCR)
	// Le clock du PIT est enable
	aPtr=(unsigned long*)kPIT_MCR;
	*aPtr&=(~2);
	
	// PIT Module Control Register (PIT_MCR)
	// Le compteur est stopp� en mode debug
	aPtr=(unsigned long*)kPIT_MCR;
	*aPtr|=0x1;
	
	// P�riode du PIT0
	// Valeur du compteur = P�riode/(1/Bus clock)
	aTmp=aPIT0PeriodeMs*kClockBus*1000;
	// Timer Load Value Register (PIT_LDVALn)
	aPtr=(unsigned long*)kPIT_LDVAL0;
	*aPtr=aTmp;
	
	// P�riode du PIT1
	// Valeur du compteur = P�riode/(1/Bus clock)
	aTmp=aPIT1PeriodeMs*kClockBus*1000;
	// Timer Load Value Register (PIT_LDVALn)
	aPtr=(unsigned long*)kPIT_LDVAL1;
	*aPtr=aTmp;
	
	// Enable de  l'interruptions PIT0 au niveau du p�riph�rique
	// Timer Control Register (PIT_TCTRLn)
	aPtr=(unsigned long*)kPIT_TCTRL0;
	*aPtr|=2;
	
	// Enable de  l'interruptions PIT1 au niveau du p�riph�rique
	// Timer Control Register (PIT_TCTRLn)
	aPtr=(unsigned long*)kPIT_TCTRL1;
	*aPtr|=2;
	
	
	// Enable de l'interruption PIT au niveau du NVIC
	// Le vecteur du PIT est le num�ro 22
	aPtr=(unsigned long*)kNVIC_ISER;
	*aPtr|=(1<<22);
	
	// Configuration de la priorit� de l'interruption PIT
	// O : plus haute priorit�
	// 3 : plus basse priorit�
	aPtr=(unsigned long*)kNVIC_IPR5;
	*aPtr&=0xFF3FFFFF;			// Reset du champ
	*aPtr|=(kPITPrio<<22);	// config du niveau d'interruption
}


//------------------------------------------------------------
// Start Pit
// aPit		: which PIT
//------------------------------------------------------------
void iPit_StartPit(PitEnum aPit)
{
	unsigned long *aPtr;
	
	// Start PIT
	if(aPit==kPit0)
		{
			// Start du PIT0
			// Timer Control Register (PIT_TCTRLn)
			aPtr=(unsigned long*)kPIT_TCTRL0;
			*aPtr|=1;
				
		}
	else if(aPit==kPit1)
		{
			// Start du PIT1
			// Timer Control Register (PIT_TCTRLn)
			aPtr=(unsigned long*)kPIT_TCTRL1;
			*aPtr|=1;
		}
}

//------------------------------------------------------------
// Lecture du flag indiquant que 500ms sont �chus
// Retour  : �tat du flag
//------------------------------------------------------------
bool iPit_GetFlag(void)
{
  return sPitFlag; 
}

//------------------------------------------------------------
// Reset du flag indiquant que 500ms sont �chus
// Retour  : �tat du flag
//------------------------------------------------------------
void iPit_ResetFlag(void)
{
	sPitFlag=false; 
}

//------------------------------------------------------------
// Interruption du PIT0 et du PIT1
//------------------------------------------------------------
void PIT_IRQHandler(void)
{
	unsigned long *aTflg0Ptr=(unsigned long*)kPIT_TFLG0;
	unsigned long *aTflg1Ptr=(unsigned long*)kPIT_TFLG1;
	static unsigned int sCounter=0;
	unsigned int i=0;
	
	// Test si l'interruption est due au PIT0
	if(((*aTflg0Ptr)&0x1)==0x1)
		{
			// Reset du flag
			*aTflg0Ptr|=0x1;
			// On doit r�aliser 500 interruptions pour optenir 500ms	
			sCounter++;
			if(sCounter>=500)
				{
					// Reset du compteur apr�s 500ms
					sCounter=0;
					
					// Flag indiquant que la p�riode PIT est �chue
					sPitFlag=true;
				}
			
			// Mise � jour des compteurs
			for(i=0;i<kNbOfDelay;i++)
				{
					if((false==sDelayTab[i].IsFree)&&(sDelayTab[i].Counter>0))
						sDelayTab[i].Counter--;
					
					if((false==sDelayTab[i].IsFree)&&(sDelayTab[i].Counter==0))
						sDelayTab[i].IsDone=true;
				}
		}
	
	
	// Test si l'interruption est due au PIT1
	if(((*aTflg1Ptr)&0x1)==0x1)
		{
			// Reset du flag
			*aTflg1Ptr|=0x1;
			
			// ... rien dans notre application
			
		}
}

//------------------------------------------------------------
// Init de la structure des delays
//------------------------------------------------------------
void iPit_InitDelay(void)
{
  unsigned char i=0;
  
  for(i=0;i<kNbOfDelay;i++)
    {
      sDelayTab[i].Counter=0;
      sDelayTab[i].IsFree=true;
      sDelayTab[i].IsDone=false;
    }
}

//------------------------------------------------------------
// Prise d'un delays
// aDelay: l'attente � effectuer (multiple de la p�riode PIT)
// Retour: le num�ro du delay attribu� ou -1 si plus de delay dispo
//------------------------------------------------------------
int iPit_GetDelay(unsigned int aDelay)
{
  unsigned int i=0;
  
  for(i=0;(i<kNbOfDelay)&&(sDelayTab[i].IsFree==false);i++);

  if(i==kNbOfDelay)
    {
      i=-1;
    }
  else
    {
      sDelayTab[i].Counter=aDelay;
      sDelayTab[i].IsFree=false;
      sDelayTab[i].IsDone=false; 
    }
  
  return i; 
}

//------------------------------------------------------------
// Contr�le si le d�lais est �chu
// aDelayNb	: le num�ro du compteur de temps
// Retour		: l'�tat du flag
//------------------------------------------------------------
bool iPit_IsDelayDone(unsigned int aDelayNb)
{
  return sDelayTab[aDelayNb].IsDone;
}

//------------------------------------------------------------
// Lib�re un compteur de temps
// aDelayNb	: le num�ro du compteur de temps
//------------------------------------------------------------
void iPit_DelayRelease(unsigned int aDelayNb)
{
  sDelayTab[aDelayNb].Counter=0;
  sDelayTab[aDelayNb].IsFree=true;
  sDelayTab[aDelayNb].IsDone=false; 
}
