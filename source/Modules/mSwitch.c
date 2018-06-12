/*
 * mSwitch.c
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */

#include "mSwitch.h"
#include "iDio.h"

//-----------------------------------------------------------------------------
// Configuration des ports IO utilisés par les interrupteurs et les poussoirs
//-----------------------------------------------------------------------------
void mSwitch_Setup(void)
{
	// Enable du clock des GPIO
	iDio_EnablePortClk();
	
	// Configuration de la fonctionnalité de chaque PIN
	iDio_PinConfig(kPortC,kPin0,kAlternate1);
	iDio_PinConfig(kPortC,kPin4,kAlternate1);
	iDio_PinConfig(kPortC,kPin6,kAlternate1);
	iDio_PinConfig(kPortC,kPin7,kAlternate1);
	iDio_PinConfig(kPortC,kPin10,kAlternate1);
	iDio_PinConfig(kPortC,kPin11,kAlternate1);
	iDio_PinConfig(kPortB,kPin18,kAlternate1);
	iDio_PinConfig(kPortB,kPin19,kAlternate1);
	iDio_PinConfig(kPortD,kPin4,kAlternate1);
	iDio_PinConfig(kPortD,kPin5,kAlternate1);
	iDio_PinConfig(kPortD,kPin6,kAlternate1);
	iDio_PinConfig(kPortD,kPin7,kAlternate1);
	
	// Configuration des Ports en entrée ou en sortie
	iDio_SetPortDirection(kPortC,kMaskIo0+kMaskIo4+kMaskIo6+kMaskIo7,kIoInput);
	iDio_SetPortDirection(kPortB,kMaskIo18+kMaskIo19,kIoInput);
	iDio_SetPortDirection(kPortD,kMaskIo4+kMaskIo5+kMaskIo6+kMaskIo7,kIoInput);
}

//-------------------------------------------------------------------------
// Lecture des interrupteurs
// aSwitchNb: quel interrupteur veut-on lire
// Retour: état du switch
//-------------------------------------------------------------------------
bool mSwitch_ReadSwitch(SwitchEnum aSwitch)
{
	bool aState=false;
		
	switch(aSwitch)
		{
			case kSw1:
				aState=iDio_GetPort(kPortB,kMaskIo18);
			break;
			case kSw2:
				aState=iDio_GetPort(kPortB,kMaskIo19);
			break;
			case kSw3:
				aState=iDio_GetPort(kPortC,kMaskIo0);
			break;
			case kSw4:
				aState=iDio_GetPort(kPortC,kMaskIo4);
			break;
			case kSw5:
				aState=iDio_GetPort(kPortC,kMaskIo6);
			break;
			case kSw6:
				aState=iDio_GetPort(kPortC,kMaskIo7);
			break;
			case kSw7:
				aState=iDio_GetPort(kPortC,kMaskIo10);
			break;
			case kSw8:
				aState=iDio_GetPort(kPortC,kMaskIo11);
			break;
		}
		
	return aState;
}

//-------------------------------------------------------------------------
// Lecture des boutons poussoir
// aPushButNb: quel boutons poussoir veut-on lire
// Retour: état du bouton poussoir
//-------------------------------------------------------------------------
bool mSwitch_ReadPushBut(PushButEnum aPushButNb)
{
	bool aState=false;
	
	switch(aPushButNb)
		{
			case kPushButSW1:
				aState=iDio_GetPort(kPortD,kMaskIo5);
			break;
			case kPushButSW2:
				aState=iDio_GetPort(kPortD,kMaskIo7);
			break;
			case kPushButSW3:
				aState=iDio_GetPort(kPortD,kMaskIo6);
			break;
			case kPushButSW4:
				aState=iDio_GetPort(kPortD,kMaskIo4);
			break;
		}

	return aState;
}

