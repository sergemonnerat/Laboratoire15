/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	mLcd.c
Auteur et Date :	Monnerat Serge 8.5.20xx

Description dans le fichier iAd.h
------------------------------------------------------------
*/


#include "iPit.h"
#include "iDio.h"
#include "mLcd.h"


// Nb de caractère max du lcd, ici 2 lignes de 16 caractères
#define kMaxLcdCarac ((unsigned char)(32))

// Nb de ligne
#define kNbOfLine ((unsigned char)(2))

// Nb de caractères par ligne
#define kNbOfChar ((unsigned char)(16))

// Définition des commandes LCD --> data 0 à data 7
// Efface le display et address counter=0
#define kClearDisplay      				((unsigned char)(0x01))	
// Address counter=0	
#define kReturnHome      					((unsigned char)(0x02))
// Lcd en mode 2 lignes,bus de 8 bits			
#define k8BitsMode_2Lines   			((unsigned char)(0x38))
// Enclenche le display sans le curseur 		
#define kDisplayOnNoCursor				((unsigned char)(0x0c))	
// Enclenche le display avec le curseur qui clignote	
#define kDisplayOnWithCursorBlink	((unsigned char)(0x0f))	
// Eteint l'écran		
#define kDisplayOff								((unsigned char)(0x08))	
// Déplacement du curseur vers la droite		
#define kMoveCursorRight					((unsigned char)(0x14))
// Déplacement du curseur vers la gauche			
#define kMoveCursorLeft						((unsigned char)(0x10))	
// Début de la ligne 2		
#define kLineJump         				((unsigned char)(0xc0)) 
// Mode incrément, pas de shift écran 		
#define kIncrModeShiftOff         ((unsigned char)(0x06)) 
// Set DDRAM address		
#define kSetDDRAMAdr              ((unsigned char)(0x80))  	
// Init value		
#define kInitValue              	((unsigned char)(0x30))  	


//-----------------------------------------------------------------------------
// Lecture du Busy Flag
// Retour : état du busy flag
//-----------------------------------------------------------------------------
static bool mLcd_ReadLcdBusy(void);

//-----------------------------------------------------------------------------
// Envoi des données au LCD
// aData: les données à transmettre au LCD
//-----------------------------------------------------------------------------
static void mLcd_SendLcdData(unsigned char);

//-----------------------------------------------------------------------------
// Envoi d'une commandes au LCD
// aCmd: la commande à transmettre au Lcd
//-----------------------------------------------------------------------------
static void mLcd_SendLcdCmd(unsigned char);

//------------------------------------------------------------
// Configuration du module mLcd
//------------------------------------------------------------
void mLcd_Setup(void)
{
	// Configuration de la fonctionnalité de chaque PIN
	iDio_PinConfig(kPortC,kPin13,kAlternate1);
	iDio_PinConfig(kPortC,kPin16,kAlternate1);
	iDio_PinConfig(kPortB,kPin9,kAlternate1);
	iDio_PinConfig(kPortB,kPin20,kAlternate1);
	iDio_PinConfig(kPortE,kPin2,kAlternate1);
	iDio_PinConfig(kPortE,kPin3,kAlternate1);
	iDio_PinConfig(kPortE,kPin6,kAlternate1);
	iDio_PinConfig(kPortE,kPin18,kAlternate1);
	iDio_PinConfig(kPortE,kPin19,kAlternate1);
	iDio_PinConfig(kPortE,kPin20,kAlternate1);
	iDio_PinConfig(kPortE,kPin21,kAlternate1);
  
	// Configuration du Port C, RS et RW
  iDio_SetPortDirection(kPortC,kMaskIo13+kMaskIo16,kIoOutput);
  
  // Configuration du Port B, E et data0
  iDio_SetPortDirection(kPortB,kMaskIo9+kMaskIo20,kIoOutput);
  
  // Configuration du PortE, datas 1...7
  iDio_SetPortDirection(kPortE,kMaskIo2+kMaskIo3+kMaskIo6+kMaskIo18+kMaskIo19+kMaskIo20+kMaskIo21,kIoOutput);
	
}

//-----------------------------------------------------------------------------  
//	Ouverture de l'interface (init du display)
//-----------------------------------------------------------------------------
void mLcd_Open(void)
{
  char   aDelayNb=0;
  
  // Delay suivant la mise sous tension de 15 ms 
  aDelayNb=iPit_GetDelay(15/kPITTime);
  while(!iPit_IsDelayDone(aDelayNb));
	iPit_DelayRelease(aDelayNb);
	
	// Envoi de la commande function set
	mLcd_SendLcdCmd(kInitValue);
	
	// Attente active de 4.1ms
	aDelayNb=iPit_GetDelay(5/kPITTime);
	while(!iPit_IsDelayDone(aDelayNb));
	iPit_DelayRelease(aDelayNb);
	
	// Envoi de la commande function set
	mLcd_SendLcdCmd(kInitValue);
	
	// Attente active de 100 us (min possible 1 ms)
	aDelayNb=iPit_GetDelay(kPITTime/kPITTime);
	while(!iPit_IsDelayDone(aDelayNb));
	iPit_DelayRelease(aDelayNb);
	
	// Envoi de la commande function set
	mLcd_SendLcdCmd(kInitValue);
	
	// ----------------------------------------
	// A partir d'ci on peut utiliser le busy
	// ----------------------------------------
	
	// Envoi de la commande function set: 2 lignes, interface 8 bits
	while(true==mLcd_ReadLcdBusy());
	mLcd_SendLcdCmd(k8BitsMode_2Lines);
	
	// Display OFF
	while(true==mLcd_ReadLcdBusy());
	mLcd_SendLcdCmd(kDisplayOff);
	
	// Efface l'écran
	while(true==mLcd_ReadLcdBusy());
	mLcd_SendLcdCmd(kClearDisplay);
	
	// Entry Mode set, mode incrément pas de shift écran
	while(true==mLcd_ReadLcdBusy());	
	mLcd_SendLcdCmd(kIncrModeShiftOff);
	
	// Display ON, pas de curseur
	while(true==mLcd_ReadLcdBusy());			
	mLcd_SendLcdCmd(kDisplayOnNoCursor);
}

//-----------------------------------------------------------------------------
// Envoi d'une commandes au LCD
// aCmd: la commande à transmettre au Lcd
//-----------------------------------------------------------------------------
static void mLcd_SendLcdCmd(unsigned char aCmd)
{
  // Bit E (start data read/write) à 0
 	iDio_SetPort(kPortB,kMaskIo9,kIoOff);
 	
  // Ecriture de la commande sur le port E et B
 	iDio_PortLcd(aCmd);
  
  // Bit RS=0 --> on sélectionne les registres d'instruction
  iDio_SetPort(kPortC,kMaskIo13,kIoOff);
  
  // Bit RW=0 --> on écrit
  iDio_SetPort(kPortC,kMaskIo16,kIoOff);
    
  // Bit E (start data read/write) à 1
  // Le bits E doit être à 1 pendant au moins 230nsec
  iDio_SetPort(kPortB,kMaskIo9,kIoOn);
  
  // Bit E (start data read/write) à 0
	// Les data sont lues à ce moment là par l'affichage
	iDio_SetPort(kPortB,kMaskIo9,kIoOff);
}

//-----------------------------------------------------------------------------
// Envoi des données au LCD
// aData: les données à transmettre au LCD
//-----------------------------------------------------------------------------
static void mLcd_SendLcdData(unsigned char aData)
{
  // Bit E (start data read/write) à 0
 	iDio_SetPort(kPortB,kMaskIo9,kIoOff);
  
  // Bit RS=1 --> on sélectionne les registres de données
  iDio_SetPort(kPortC,kMaskIo13,kIoOn);
  
  // Bit RW=0 --> on écrit
  iDio_SetPort(kPortC,kMaskIo16,kIoOff);
  
  // Ecriture la donnée sur le port E
  iDio_PortLcd(aData);
    
  // Bit E (start data read/write) à 1
  // Le bits E doit être à 1 pendant au moins 230nsec
  iDio_SetPort(kPortB,kMaskIo9,kIoOn);
  
  // Bit E (start data read/write) à 0
  // Les data sont lues à ce moment là par l'affichage
	iDio_SetPort(kPortB,kMaskIo9,kIoOff);
}

//-----------------------------------------------------------------------------
// Lecture du Busy Flag
// Retour : état du busy flag
//-----------------------------------------------------------------------------
static bool mLcd_ReadLcdBusy(void)
{
 	bool aVal;
 	
 	// Configuration du Port B --> data 0
 	iDio_SetPortDirection(kPortB,kMaskIo20,kIoInput);
 	// Configuration du PortE, datas 1...7
 	iDio_SetPortDirection(kPortE,kMaskIo2+kMaskIo3+kMaskIo6+kMaskIo18+kMaskIo19+kMaskIo20+kMaskIo21,kIoInput);

 	// Bit E (start data read/write) à 0
 	iDio_SetPort(kPortB,kMaskIo9,kIoOff);

	// Bit RS=0 --> on sélectionne les registres d'instruction
  iDio_SetPort(kPortC,kMaskIo13,kIoOff);
  
  // Bit RW=1, read
  iDio_SetPort(kPortC,kMaskIo16,kIoOn);
  	
	// Bit E (start data read/write) à 1
	// E (start read) doit être inséré pendant au moins 230ns
	// Les données sont valides après 120ns.
	iDio_SetPort(kPortB,kMaskIo9,kIoOn);
 	
 	// Lecture du data 7 --> busy flag
 	// On lit le flag avant de redescendre E afin d'être sur
 	// de ne pas le manquer car il est valide uniquement pendant
 	// 300 ns après que redescende à 0
 	aVal=iDio_GetPort(kPortE,kMaskIo19);
 	
 	// Bit E (start data read/write) à 0
 	iDio_SetPort(kPortB,kMaskIo9,kIoOff);
 	
 	// Configuration du Port B --> data 0
 	iDio_SetPortDirection(kPortB,kMaskIo20,kIoOutput);
 	// Configuration du PortE, datas 1...7
 	iDio_SetPortDirection(kPortE,kMaskIo2+kMaskIo3+kMaskIo6+kMaskIo18+kMaskIo19+kMaskIo20+kMaskIo21,kIoOutput);

 	// Retour de l'état du busy flag
 	return aVal; 
}


//-----------------------------------------------------------------------------
//	Ecriture sur le LCD complet
// aChar: Pointeur sur la chaîne de charactère à afficher 32 caractères
//-----------------------------------------------------------------------------
void mLcd_WriteEntireDisplay(char *aChar)
{

	unsigned int i;
	
	// Attente du busy flag=0
	while(true==mLcd_ReadLcdBusy());
	
	// AC à 0, début de la 1ère ligne
	mLcd_SendLcdCmd(kReturnHome);
	
	// Envoi des 32 caractères à l'affichage
	// Attention il faut position manuellement l'Address Counter
	// au début de la 2e ligne lors de l'écriture de cette dernière
	for (i=0;i<kMaxLcdCarac;i++) 
		{
	  	// Attente du busy=0
			while(true==mLcd_ReadLcdBusy());
			
			// Envoi du caractère au LCD
			mLcd_SendLcdData(*aChar);
			
			// Si fin de 1ère ligne on passe à la 2e
			if((kNbOfChar-1)==i)
				{
					// Attente du busy flag=0
					while(true==mLcd_ReadLcdBusy());
	
					// 2e ligne
					mLcd_SendLcdCmd(kLineJump);
				}
			
			// Caractère suivant
			aChar++;
	  }
}

//-----------------------------------------------------------------------------
// Ecriture sur le LCD d'un caractère à un endroit précis
// aChar: Le caractère que l'on veut afficher
// aXpos: Position horizontale du 1er caractère
// aYPos: Position verticale du 1er caractère  
//-----------------------------------------------------------------------------
void mLcd_Write(unsigned char aChar,unsigned char aXpos,unsigned char aYPos)
{
	unsigned int aTmp=0;
		
	// Modification de l'Address Counter AC
	// Calcul de l'adresse DDRAM
	aTmp=kSetDDRAMAdr+aXpos+(aYPos*0x40);
	
	// Attente du busy flag=0
	while(true==mLcd_ReadLcdBusy());
	
	// Set DDRAM Address Counter
	mLcd_SendLcdCmd(aTmp);
	
	// Attente du busy flag=0
	while(true==mLcd_ReadLcdBusy());
	
	// Envoi du caractère au LCD
	mLcd_SendLcdData(aChar);
}
