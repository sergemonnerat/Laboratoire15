/*
 * mDelay.h
 *
 *  Created on: Apr 3, 2014
 *      Author: serge.monnerat
 */

#ifndef MDELAY_H_
#define MDELAY_H_

//------------------------------------------------------------
// Configuration du module mDelay
//------------------------------------------------------------
void mDelay_Setup(void);

//------------------------------------------------------------
// Lecture du flag indiquant que 500ms sont échus
// Retour  : état du flag
//------------------------------------------------------------
bool mDelay_GetFlag(void);

//------------------------------------------------------------
// Reset du flag indiquant que 500ms sont échus
// Retour  : état du flag
//------------------------------------------------------------
void mDelay_ResetFlag(void);

//------------------------------------------------------------
// Configuration des compteurs de temps
// aDelay	: le temps à écouler
// Retour	: le numéro du compteur de temps (-1) si plus de 
//					compteurs libres
//------------------------------------------------------------
int mDelay_GetDelay(unsigned int aDelay);

//------------------------------------------------------------
// Contrôle si le délais est échu
// aDelayNb	: le numéro du compteur de temps
// Retour		: l'état du flag
//------------------------------------------------------------
bool mDelay_IsDelayDone(unsigned int aDelayNb);

//------------------------------------------------------------
// Libère un compteur de temps
// aDelayNb	: le numéro du compteur de temps
//------------------------------------------------------------
void mDelay_DelayRelease(unsigned int aDelayNb);

#endif /* MDELAY_H_ */
