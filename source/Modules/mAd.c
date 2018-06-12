/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	mAd.c
Auteur et Date :	Monnerat Serge 20.1.20xx

Description dans le fichier mAd.h
------------------------------------------------------------
*/


#include "mAd.h"
#include "iAd.h"

//-----------------------------------------------------------------------
// Configuration du module ADC
//-----------------------------------------------------------------------
void mAd_Setup(void)
{
	iAd_Config();
	iAd_Cal();
	iAd_Config();
}

//-----------------------------------------------------------------------
// ADC module open
//-----------------------------------------------------------------------
void mAd_Open(void)
{}

//-----------------------------------------------------------------------
// ADC module close
//-----------------------------------------------------------------------
void mAd_Close(void)
{}


//-----------------------------------------------------------------------
// Start et lecture du résultat
//-----------------------------------------------------------------------
UInt16 mAd_Read(ADCInputEnum aAdcInp)
{
	UInt16 aRet;
	
	switch(aAdcInp)
	{
		case kTemp:
			iAd_SelectChannel(kADC_SE9);
			while(iAd_GetCompleteConvStatus()==false);
			aRet=iAd_GetResult();
		break;
		case kP1:
			iAd_SelectChannel(kADC_SE12);
			while(iAd_GetCompleteConvStatus()==false);
			aRet=iAd_GetResult();
		break;
		case kP2:
			iAd_SelectChannel(kADC_SE13);
			while(iAd_GetCompleteConvStatus()==false);
			aRet=iAd_GetResult();
		break;
	}
	
	return aRet;
}
