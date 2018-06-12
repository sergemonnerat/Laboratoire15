/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name : 	mRs232.h	
Author and date :	Monnerat Serge 06.06.20xx

Goal : outils afin de pouvoir exploiter le périphérique RS232

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#ifndef __MRS232__
#define __MRS232__

#include "def.h"

//-------------------------------------------------------------------------
// Configuration du module SCI   
//-------------------------------------------------------------------------
void mRs232_Setup(void);

//-------------------------------------------------------------------------
// Ouverture des interfaces    
//-------------------------------------------------------------------------
void mRs232_Open(void);

//-------------------------------------------------------------------------
// Envoi d'un caractère par RS232
// aChar	: la caractère à transmettre
//-------------------------------------------------------------------------
void mRs232_WriteChar(Int8 aChar);

//-------------------------------------------------------------------------
// Envoi d'une chaîne de caractères par RS232
// La tramsmission s'arrête au caractère NULL
// *aDataPtr	: pointeur sur la chaîne de caractères à transmettre, la chaîne
//              doit-être terminée par le caractère NULL("ma chaîne")
//-------------------------------------------------------------------------
void mRs232_WriteString(Int8 *aDataPtr);

//-----------------------------------------------------------------------------
// Lecture d'un byte du buffer de réception. Le buffer est rempli par 
// l'interruption Receive data reg full
// *aBytePtr	  : pointeur sur la variable recevant la donnée
// Retour 	    : true --> le buffer est vide, false le buffer est pas vide
//-----------------------------------------------------------------------------
bool mRs232_ReadDataFromBuffer(UInt8 *aBytePtr);


#endif
