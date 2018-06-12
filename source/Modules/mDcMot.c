/*
------------------------------------------------------------
Copyright 2003-201x Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	mDcMot.c
Author and date :	Monnerat Serge 17 juin 2014

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "mDcMot.h"
#include "iTPM.h"

//-----------------------------------------------------------------------------
// DC motor module setup
//-----------------------------------------------------------------------------
void mDcMot_Setup(void)
{
	// Timer setup
	iTPM_Config();
}

//-----------------------------------------------------------------------------
// DC motor module start
//-----------------------------------------------------------------------------
void mDcMot_Open(void)
{
	
}

//-----------------------------------------------------------------------------
// DC motor module stop
//-----------------------------------------------------------------------------
void mDcMot_Close(void)
{
	
}

//-----------------------------------------------------------------------------
// Set DC motor duty counter clockwise
// aDuty: pwm duty cycle (0.0 to 1.0 --> 0% to 100%)
//-----------------------------------------------------------------------------
void mDcMot_SetCWDuty(float aDuty)
{
	iTPM_UpdatePWM(kPwmCh3,0);
	iTPM_UpdatePWM(kPwmCh4,aDuty);
}

//-----------------------------------------------------------------------------
// Set DC motor duty counter anti clockwise
// aDuty: pwm duty cycle (0.0 to 1.0 --> 0% to 100%)
//-----------------------------------------------------------------------------
void mDcMot_SetACWDuty(float aDuty)
{
	iTPM_UpdatePWM(kPwmCh3,aDuty);
	iTPM_UpdatePWM(kPwmCh4,0);
}

//-----------------------------------------------------------------------------
// Lecture et calcul de la vitesse
//-----------------------------------------------------------------------------
float mDcMot_GetFrequence(void)
{
	return iTPM_GetFrequenceCh0();
}
