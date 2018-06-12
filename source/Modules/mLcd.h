/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	mLcd.h	
Auteur et Date :	Monnerat Serge 8.5.20xx

But : module LCD permettant de piloter l'écran LCD

Modifications
Date		Faite	Ctrl		Description
------------------------------------------------------------
*/

#ifndef MLCD_H_
#define MLCD_H_

#include "def.h"

//------------------------------------------------------------
// Configuration du module mLcd
//------------------------------------------------------------
void mLcd_Setup(void);

//-----------------------------------------------------------------------------  
//	Ouverture de l'interface (init du display)
//-----------------------------------------------------------------------------
void mLcd_Open(void);

//-----------------------------------------------------------------------------
//	Ecriture sur le LCD complet
// aChar: Pointeur sur la chaîne de charactère à afficher 32 caractères
//-----------------------------------------------------------------------------
void mLcd_WriteEntireDisplay(char *aChar);

//-----------------------------------------------------------------------------
// Ecriture sur le LCD d'un caractère à un endroit précis
// aChar: Le caractère que l'on veut afficher
// aXpos: Position horizontale du 1er caractère
// aYPos: Position verticale du 1er caractère  
//-----------------------------------------------------------------------------
void mLcd_Write(unsigned char aChar,unsigned char aXpos,unsigned char aYPos);


#endif /* MLCD_H_ */
