/*
 * iPit.h
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#ifndef __iPit__
#define __iPit__

#include "def.h"

// Enumération des PIT
typedef enum
{
	kPit0,
	kPit1
}PitEnum;

//------------------------------------------------------------
// Configuration du PIT
// aPIT0PeriodeMs  	: période en ms du PIT0
// aPIT1PeriodeMs  	: période en ms du PIT1
//------------------------------------------------------------
void iPit_Config(unsigned int aPIT0PeriodeMs,unsigned int aPIT1PeriodeMs);

//------------------------------------------------------------
// Start Pit
// aPit		: which PIT
//------------------------------------------------------------
void iPit_StartPit(PitEnum aPit);

//------------------------------------------------------------
// Lecture du flag indiquant que 500ms sont échus
// Retour  : état du flag
//------------------------------------------------------------
bool iPit_GetFlag(void);

//------------------------------------------------------------
// Reset du flag indiquant que 500ms sont échus
// Retour  : état du flag
//------------------------------------------------------------
void iPit_ResetFlag(void);

//------------------------------------------------------------
// Init de la structure des delays
//------------------------------------------------------------
void iPit_InitDelay(void);

//------------------------------------------------------------
// Prise d'un delays
// aDelay: l'attente à effectuer (multiple de la période PIT)
// Retour: le numéro du delay attribué ou -1 si plus de delay dispo
//------------------------------------------------------------
int iPit_GetDelay(unsigned int aDelay);

//------------------------------------------------------------
// Contrôle si le délais est échu
// aDelayNb	: le numéro du compteur de temps
// Retour		: l'état du flag
//------------------------------------------------------------
bool iPit_IsDelayDone(unsigned int aDelayNb);

//------------------------------------------------------------
// Libère un compteur de temps
// aDelayNb	: le numéro du compteur de temps
//------------------------------------------------------------
void iPit_DelayRelease(unsigned int aDelayNb);

#endif
