/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	mIcOc.c
Auteur et Date :	Monnerat Serge 11.01.20xx

Description dans le fichier mIcOc.h
------------------------------------------------------------
*/

#include "mIcOc.h"
#include "iTPM.h"

//-----------------------------------------------------------------------------
// Configuration du module TIMER
//-----------------------------------------------------------------------------
void mIcOc_Setup(void)
{
	// Configuration du TIMER
	iTPM_Config();
	
}

//-----------------------------------------------------------------------------
// Ouverture de l'interface    
//-----------------------------------------------------------------------------
void mIcOc_Open(void)
{

}

//-----------------------------------------------------------------------------
// Fermeture de l'interface   
//-----------------------------------------------------------------------------
void mIcOc_Close(void)
{

}


//------------------------------------------------------------
// Lecture de la fréquence calculée dans l'interruption
//------------------------------------------------------------
float mIcOc_GetFrequence(void)
{
	return iTPM_GetFrequence();
}

//------------------------------------------------------------
// Modification de la fréquence de OC
//------------------------------------------------------------
void mIcOc_SetFrequence(UInt32 aFreq)
{
	iTPM_SetFrequence(aFreq);
}
