/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	mIcOc.h	
Auteur et Date :	Monnerat Serge 11.01.20xx

But : Module permettant l'exploitation des fonctions Input Capture et
			Output Compare

Modifications
Date		Faite	Ctrl		Description
------------------------------------------------------------
*/

#ifndef __MICOC__
#define __MICOC__

#include "def.h"

//-----------------------------------------------------------------------------
// Configuration du module TIMER
//-----------------------------------------------------------------------------
void mIcOc_Setup(void);

//-----------------------------------------------------------------------------
// Ouverture de l'interface    
//-----------------------------------------------------------------------------
void mIcOc_Open(void);

//-----------------------------------------------------------------------------
// Fermeture de l'interface   
//-----------------------------------------------------------------------------
void mIcOc_Close(void);

//------------------------------------------------------------
// Lecture de la fréquence calculée dans l'interruption
//------------------------------------------------------------
float mIcOc_GetFrequence(void);

//------------------------------------------------------------
// Modification de la fréquence de OC
//------------------------------------------------------------
void mIcOc_SetFrequence(UInt32 aFreq);


#endif
