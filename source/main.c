/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	main.c
Auteur et Date :	Monnerat Serge 11.01.20xx

Description dans le fichier 
------------------------------------------------------------
*/

#include "mLeds.h"
#include "mSwitch.h"
#include "mDelay.h"
#include "mLcd.h"
#include "mRs232.h"
#include "mCpu.h"
#include "mMeteo.h"
#include "mAd.h"
#include "mDcMot.h"
#include "def.h"
#include "stdio.h"

// ----------------------------------------------------------
// Config en boucle ouverte ou avec PI
// ----------------------------------------------------------
#define kOpenLoop 1

// ----------------------------------------------------------
// Déclaration des variables
// ----------------------------------------------------------
#if(kOpenLoop==1)
// Message affiché sur le LCD
static Int8 sLcdMsg[]={'%','=','0','.','0','0',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
												'V','=','0','0','0',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
#else
// Message affiché sur le LCD
static Int8 sLcdMsg[]={'P','=','0','.','0','0','0','0','I','=','0','.','0','0','0','0',
												'V','=','0','0','0','0',' ',' ','C','=',' ',' ',' ',' ',' ',' '};
#endif

// Valeur des poussoirs de la boucle infoinie précédente
static bool  sOldSw1;
static bool  sOldSw2;
static bool  sOldSw3;
static bool  sOldSw4;



#if (kOpenLoop==0)
//static UInt16 sTab[8];

//static float sInter2=0;
//static UInt16 sAdTab[8];
// Structure de données utilisées par le régulateur PI
typedef struct
{
  float LimTotLow;
  float LimTotHigh;
  float GainP;
  float GainI;
}RegPIStruct;   

static RegPIStruct sRegPI;

// Déclaration des prototypes
static float RegPI(RegPIStruct *theRegPIStruct,float *theIntPart,float theErr);
#endif

// Déclaration des prototypes
static float LowPassFilter (float x, float r, float* w);

//-----------------------------------------------------------------------------
// Programme principal
//-----------------------------------------------------------------------------
int main(void) 
{   
	Int8 aCharTab[10];
	bool  aSw;
	static float sFreq;
	static float sInter=0;
		
#if (kOpenLoop==1)
	// Rapport cyclique
	static float sDuty=0.3;
	static bool  sEnable;
#else
	Int8 aDelayNb;
	float aC;
	UInt16 aPot;
	float aIntPart;
	float aErr;
	float aDuty;
#endif
		
	// Congiguration générale de la CPU
	mCpu_Setup();
	
	// Configuration du module LEDS
	mLeds_Setup();
	
	// Configuration du module Switch
	mSwitch_Setup();
	
	// Configuration du module Delay
	mDelay_Setup();
	
	// Configuration du module LCD
	mLcd_Setup();
	
	// Configuration du module Rs232
	mRs232_Setup();
	
	// Configuration du module AD
	mAd_Setup();
	
	// Dc Mot module setup
	mDcMot_Setup();
	
	// Enable des interruptions au niveau de la CPU
	// --> Primask
	EnableInterrupts;
	
	// Ouverture du module LCD
	mLcd_Open();
	
	// Ouverture du module Rs232
	mRs232_Open();
	
	// Initialisation des valeurs par défaut 
#if (kOpenLoop==0)	
	sRegPI.LimTotLow=0;
	sRegPI.LimTotHigh=1;
	sRegPI.GainP=0.002;
	sRegPI.GainI=0.00005;
	// Demande d'un delay de 10ms
	aDelayNb=mDelay_GetDelay(10/kPITTime);
#endif
	

#if (kOpenLoop==1)	
	// Par défaut
	sLcdMsg[28]='D';
	sLcdMsg[29]='i';
	sLcdMsg[30]='s';
#endif 
	  
	while(1)
		{		  
// -----------------------------------------------------------
// Mode boucle ouverte
// -----------------------------------------------------------
#if (kOpenLoop==1)
			
			// Lecture du poussoir 1 -> enable pont en H
			aSw=mSwitch_ReadPushBut(kPushButSW1);
			if((true==aSw)&&(false==sOldSw1))
				{
					sLcdMsg[28]='E';
					sLcdMsg[29]='n';
					sLcdMsg[30]=' '; 
					sEnable=true;
				}
			
			// Sauvegarde de la valeur
			sOldSw1=aSw;
			
			// Lecture du poussoir 2 -> disable pont en H
			aSw=mSwitch_ReadPushBut(kPushButSW2);
			if((true==aSw)&&(false==sOldSw2))
				{
					sLcdMsg[28]='D';
					sLcdMsg[29]='i';
					sLcdMsg[30]='s'; 
					sEnable=false;
				}
			
			// Sauvegarde de la valeur
			sOldSw2=aSw; 
			
			// Lecture du poussoir 3, incrémentation du duty
			aSw=mSwitch_ReadPushBut(kPushButSW3);
			if((true==aSw)&&(false==sOldSw3))
				{
					sDuty+=0.01;
					if(sDuty>=1.0)
						sDuty=1;
				}
			
			// Sauvegarde de la valeur
			sOldSw3=aSw; 
			
			// Lecture du poussoir 4, décrémentation du duty
			aSw=mSwitch_ReadPushBut(kPushButSW4);
			if((true==aSw)&&(false==sOldSw4))
				{
					sDuty-=0.01;
					if(sDuty<0)
						sDuty=0;
				}
			
			// Sauvegarde de la valeur
			sOldSw4=aSw;
			
			// Conversion en chaîne de caractères 
			sprintf(aCharTab,"%04.2f",sDuty);
			
			sLcdMsg[2]=aCharTab[0];
			sLcdMsg[3]=aCharTab[1];
			sLcdMsg[4]=aCharTab[2];
			sLcdMsg[5]=aCharTab[3];
			
			// Lecture de la vitesse
			sFreq=mDcMot_GetFrequence();

			// Filtrage de la vitesse
			sFreq=LowPassFilter(sFreq,1000.0,&sInter);
		
			// Conversion en chaîne de caractères
			sprintf(aCharTab,"%05.1f",sFreq);
			
			sLcdMsg[18]=aCharTab[0];
			sLcdMsg[19]=aCharTab[1];
			sLcdMsg[20]=aCharTab[2];
			sLcdMsg[21]=aCharTab[3];
			sLcdMsg[22]=aCharTab[4];
			
			// Ecriture d'un message sur le LCD en entier
			mLcd_WriteEntireDisplay(sLcdMsg);
			
			// Envoi des nouvelles consignes PWM
			if(mSwitch_ReadSwitch(kSw1)==true)
				{
					if(sEnable==true)
						{
							mDcMot_SetCWDuty(sDuty);
						}
					else
						{
							mDcMot_SetCWDuty(0);
						}
					
				}
			else
				{
					if(sEnable==true)
						{
							mDcMot_SetACWDuty(sDuty);
						}
					else
						{
							mDcMot_SetACWDuty(0);
						}
				}

// -----------------------------------------------------------
// Mode boucle fermée -> Regulateur PI
// -----------------------------------------------------------
#else
			 // Lecture du poussoir 1
			aSw=mSwitch_ReadPushBut(kPushButSW1);
			if((true==aSw)&&(false==sOldSw1))
				{
					sRegPI.GainP+=0.0001;
				}
			
			// Sauvegarde de la valeur
			sOldSw1=aSw; 
			
			// Lecture du poussoir 2
			aSw=mSwitch_ReadPushBut(kPushButSW2);
			if((true==aSw)&&(false==sOldSw2))
				{
					sRegPI.GainP-=0.0001;
				}
			
			// Sauvegarde de la valeur
			sOldSw2=aSw;
			
			// Lecture du poussoir 3
			aSw=mSwitch_ReadPushBut(kPushButSW3);
			if((true==aSw)&&(false==sOldSw3))
				{
					sRegPI.GainI+=0.0001;
				}
			
			// Sauvegarde de la valeur
			sOldSw3=aSw; 
			
			// Lecture du poussoir 4
			aSw=mSwitch_ReadPushBut(kPushButSW4);
			if((true==aSw)&&(false==sOldSw4))
				{
					sRegPI.GainI-=0.0001;
				}
			
			// Sauvegarde de la valeur
			sOldSw4=aSw;
			
			// Conversion en chaîne de caractères 
			sprintf(aCharTab,"%01.4f",sRegPI.GainP);
			
			sLcdMsg[2]=aCharTab[0];
			sLcdMsg[3]=aCharTab[1];
			sLcdMsg[4]=aCharTab[2];
			sLcdMsg[5]=aCharTab[3];
			sLcdMsg[6]=aCharTab[4];
			sLcdMsg[7]=aCharTab[5];
			
			// Conversion en chaîne de caractères
			sprintf(aCharTab,"%01.4f",sRegPI.GainI);
			
			sLcdMsg[10]=aCharTab[0];
			sLcdMsg[11]=aCharTab[1];
			sLcdMsg[12]=aCharTab[2];
			sLcdMsg[13]=aCharTab[3];
			sLcdMsg[14]=aCharTab[4];
			sLcdMsg[15]=aCharTab[5];
			
			// Lecture de la vitesse
			sFreq=mDcMot_GetFrequence();

			// Filtrage de la vitesse
			sFreq=LowPassFilter(sFreq,10.0,&sInter);
		
			// Conversion en chaîne de caractères
			sprintf(aCharTab,"%05.1f",sFreq);
			
			sLcdMsg[18]=aCharTab[0];
			sLcdMsg[19]=aCharTab[1];
			sLcdMsg[20]=aCharTab[2];
			sLcdMsg[21]=aCharTab[3];
			
			// Lecture de la vitesse
			aPot=mAd_Read(kP1);
			
			// Transformation en consigne de vitesse --> max 600 tr/mn
			aC=(aPot*600.0)/4096.0;
			
			// Conversion en chaîne de caractères
			sprintf(aCharTab,"%05.1f",aC);
			sLcdMsg[26]=aCharTab[0];
			sLcdMsg[27]=aCharTab[1];
			sLcdMsg[28]=aCharTab[2];
			
			// Ecriture d'un message sur le LCD en entier
			mLcd_WriteEntireDisplay(sLcdMsg);
			
			// Régulation toutes les 10ms
			if(mDelay_IsDelayDone(aDelayNb)==true)
				{			    
					// Calcul de l'erreur entre consigne et mesure
					aErr=(aC-sFreq);
					
					// Régulateur PI
					aDuty=RegPI(&sRegPI,&aIntPart,aErr);
					
					// envoi des nouvelles consignes PWM
					mDcMot_SetCWDuty(aDuty);
					
					// Redonne le delay au système
					mDelay_DelayRelease(aDelayNb);
					
					// Delay de 4.096 ms
					aDelayNb=mDelay_GetDelay(10/kPITTime);
				}
#endif
		}
}

//-----------------------------------------------------------------------------
// Filtre passe bas du 1er ordre
// ce filtre utilise une valeur intermédiaire "w" pour augmenter la précision
// du calcul selon la formule suivante
// w(k) = x(k) + w(k-1) - w(k-1)/r
// y(k) = w(k)/r
//  avec r = TempsEchant. x Fréq. de coupure x 2 x PI
//
// paramètres       : x et r par valeur, w par adresse
// valeur de retour : y
//-----------------------------------------------------------------------------
static float LowPassFilter (float x, float r, float *w)
{
	// calcul la nouvelle valeur de w
	*w = x+(*w)-((*w)/r);
	
	// retourne la valeur filtrée
	return ((*w)/r);
}

//-----------------------------------------------------------------------------
// Régulateur Proportionnel-Intégral avec limitation de la sortie et annulation 
// de la dérive de l'intégrateur lorsque la sortie à atteint sa valeur de limitation 
// theRegPIStruct: adr. de la struct. contenant les limit. et gains du PI
// theIntPart    : addresse de la partie intégrale
// theErr        : différence entre mesure et consigne
// Temps         : appel + exécution + return = 48.060 us à 24 MHz   
//----------------------------------------------------------------------------- 
#if (kOpenLoop==0)
static float RegPI(RegPIStruct *theRegPIStruct,float *theIntPart,float theErr)
{
  float y;
  float ysat;
  float eLim;
 
  // Calcul de la sortie du régulateur avant saturation y = (Err * GainP) + partie int.
  y=(theErr*(theRegPIStruct->GainP))+(*theIntPart);
 
  // Saturation  du résultat
  if(y>theRegPIStruct->LimTotHigh)
    {
      ysat=theRegPIStruct->LimTotHigh;
    }
  else if(y<theRegPIStruct->LimTotLow)
    {
      ysat=theRegPIStruct->LimTotLow;
    }
  else
    {
      ysat=y;
    }
 
  if((theRegPIStruct->GainP>0)&&(theRegPIStruct->GainI>0))
    {
      // Calcul de l'écart de réglage fictif, permettant d'éviter une dérive de l'intégrale
      eLim= (y-ysat)/((theRegPIStruct->GainP)+theRegPIStruct->GainI);
    }
  else
    {
      eLim=0;
    }
     
  // partie intégrale = partie intégrale + (erreur - écart de réglage fictif) * GainI
  (*theIntPart)=((theErr-eLim)*(theRegPIStruct->GainI))+(*theIntPart);
 
  return ysat;                                     
}
#endif



