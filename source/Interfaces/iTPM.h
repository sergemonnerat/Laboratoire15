/*
------------------------------------------------------------
Copyright 2003-2010 Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name : 	iFlextimer.h	
Author and date :	Monnerat Serge 1 mars 2012

Goal : 

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $


-----------------------------------------------------------------------------
*/

#ifndef __ITPM__
#define __ITPM__

#include "def.h"


// Pwm channels enum
typedef enum
{
	kPwmCh3,
	kPwmCh4
}PwmChannelEnum;


//------------------------------------------------------------
// TPM setup
//------------------------------------------------------------
void iTPM_Config(void);

//------------------------------------------------------------
// Set PWM duty
//------------------------------------------------------------
void iTPM_UpdatePWM(PwmChannelEnum aChannel, float aDuty);

//------------------------------------------------------------
// Lecture du nb de step entre 2 flancs --> canal 0
//------------------------------------------------------------
float iTPM_GetFrequenceCh0(void);

//------------------------------------------------------------
// Lecture du nb de step entre 2 flancs --> canal 1
//------------------------------------------------------------
float iTPM_GetFrequenceCh1(void);

#endif
