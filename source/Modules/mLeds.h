/*
 * mLeds.h
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#ifndef MLEDS_H_
#define MLEDS_H_

#include"def.h"

// Masque des LED
typedef enum
{
  kMaskLed1=0x1,
  kMaskLed2=0x2,
  kMaskLed3=0x4,
  kMaskLed4=0x8,
  kMaskLed5=0x10,
  kMaskLed6=0x20,
  kMaskLed7=0x40,
  kMaskLed8=0x80,
  kMaskLedAll=0xFF
}LedMaskEnum;

// Etat des LED
typedef enum
{
  kLedOff,
  kLedOn
}LedStateEnum;

//-----------------------------------------------------------------------------
// Configuration des ports IO utilisés par les LEDS
//-----------------------------------------------------------------------------
void mLeds_Setup(void);

//-----------------------------------------------------------------------------
// On Off des LED
// aMask:   choix des LED sur lesquelles on veut agir (si bit à 1)
// aState:  choix de l'état des LED désirés
//-----------------------------------------------------------------------------
void mLeds_Write(LedMaskEnum aMask,LedStateEnum aState);

#endif /* MLEDS_H_ */
