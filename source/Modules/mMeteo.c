/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	mMeteo.c
Auteur et Date :	Monnerat Serge 8.5.20xx

Description dans le fichier mMeteo.h
------------------------------------------------------------
*/


#include "mMeteo.h"
#include "iI2c.h"

// Adresse du capteur de pression/altitude/température MPL3115A2
#define kMpl3115A2WriteAdr 	0xC0
#define kMpl3115A2ReadAdr 	0xC1
// Adresse du capteur d'humidité HTU21
#define kHTU21WriteAdr 	0x80
#define kHTU21ReadAdr 	0x81

// Flag indiquant une nouvelle pression ou altitude disponible
#define kNewAltorPress 0x04
// Flag indiquant une nouvelle température disponible
#define kNewTemp 0x02

// Prototype des fonctions statiques
static bool mMeteo_GetData(MeteoRegisterEnum aReg, UInt8* aData);
static bool mMeteo_SetData(MeteoRegisterEnum aReg,UInt8 aVal);
static bool mMeteo_HTU21GetData(HTU21RegisterEnum aReg, UInt16* aData, UInt8* aCrc);

//-----------------------------------------------------------------------------
// Configuration du module      
//-----------------------------------------------------------------------------
void mMeteo_Setup(void)
{	
  // Configure le diviseur du clock
	iI2C_Config();
}

//-----------------------------------------------------------------------------
// Ouverture du module      
//-----------------------------------------------------------------------------
void mMeteo_Open(void)
{
	bool  aRet;
	UInt8 aData;
	UInt16 aData2;
	
	// Enable IIC
  iI2C_Enable(); 
  
  // Lecture de l'ID du capteur --> test
  aRet=mMeteo_GetData(kWHO_AM_I, &aData);
  
  // Configuration du mode actif et lecture de l'altitude, delta T 6ms
  aRet=mMeteo_SetData(kCTRL_REG1,0x81);
  
  // Configuration du mode polling, enable des flags nouvelle pression et nouvelle temp
   aRet=mMeteo_SetData(kPT_DATA_CFG,0x07);   
}

//-----------------------------------------------------------------------------
// Fermeture du module      
//-----------------------------------------------------------------------------
void mMeteo_Close(void)
{
  // Disable IIC
  iI2C_Disable();	
}

//-----------------------------------------------------------------------------
// Lecture d'un registre du capteur MPL3115A2 (pression/altitude et temp)
// aReg:    le registre que l'on veut lire
// aData:   l'adresse du variable dans laquelle on écrit le contenu du registre
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
static bool mMeteo_GetData(MeteoRegisterEnum aReg, UInt8* aData)
{
	bool aNoAck=false;
		
	// Disable transmit ACK
  iI2C_SetAckMode(kNoAck);

  // Attend que le bus soit libre
  while(true==iI2C_ReadStatus(kBUSY)); 
	
	//-----------------------------------------------------------------------------
	// D'abords en WRITE afin de transmettre le registre 
	// que l'on veut lire ainsi que l'adresse du slave
	//-----------------------------------------------------------------------------
		
	// Début de la transmission --> mode write et START condition 
  iI2C_TxRxSelect(kTxMode); 
  iI2C_SetStartState();
  
  // Transmission de l'adresse en WRITE du slave dans le registre de données
	// --> obligatoire protocolle I2C, le slave doit d'abords répondre à son adresse
	iI2C_SendData(kMpl3115A2WriteAdr);

	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	if(aNoAck==true)
		{
			// FIN de la lecture
			iI2C_TxRxSelect(kTxMode);
			iI2C_SetAckMode(kNoAck);
			iI2C_SetStopState();
			return false;
		}
	
	// Transmission de l'adresse du registre que l'on veut lire
	iI2C_SendData(aReg);
	
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	if(aNoAck==true)
		{
			// FIN de la lecture
			iI2C_TxRxSelect(kTxMode);
			iI2C_SetAckMode(kNoAck);
			iI2C_SetStopState();
			return false;
		}
			
	// Nouvelle condition START 
	iI2C_SetRepeatedStartSate();
	
	//-----------------------------------------------------------------------------
	// Passage en READ
	//-----------------------------------------------------------------------------
	
	// Transmission de l'adresse en READ du slave dans le registre de données
	// --> obligatoire protocolle I2C, le slave doit d'abords répondre à son adresse
	iI2C_SendData(kMpl3115A2ReadAdr);
	
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	if(aNoAck==true)
		{
			// FIN de la lecture
			iI2C_TxRxSelect(kTxMode);
			iI2C_SetAckMode(kNoAck);
			iI2C_SetStopState();
			return false;
		}
	 
	// Passage en mode READ 
	// --> En lecture c'est au master d'envoyer le ACK mais comme on 
	// lit q'un byte pas besoin de ACK
	//iI2C_EnableTxAck();
	iI2C_TxRxSelect(kRxMode); 
	
	// Lecture qui ne sert à rien --> permet l'émission de l'horloge nécessaire au
	// slave afin de transmettre sa donnée
	iI2C_ReadData();
	
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	 
	// FIN de la lecture
	// Génération de la condition STOP
	// Obligatoire de faire le STOP avant la lecture pour 
	// ne pas émettre de clock à nouveau!
	iI2C_TxRxSelect(kTxMode);
	iI2C_SetAckMode(kNoAck);
	iI2C_SetStopState();
	
	// Lecture de la valeur du registre demandé
	*aData=iI2C_ReadData();
	
	return true;	
}

//-----------------------------------------------------------------------------
// Ecriture d'un registre du capteur MPL3115A2 (pression/altitude et temp)
// aReg:    le registre que l'on veut écrire
// aData:   le contenu du registre
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
static bool mMeteo_SetData(MeteoRegisterEnum aReg,UInt8 aVal)
{
	bool aNoAck=false;
	
	// Disable transmit ACK
  iI2C_SetAckMode(kNoAck);

  // Attend que le bus soit libre
  while(true==iI2C_ReadStatus(kBUSY)); 
	
	//-----------------------------------------------------------------------------
	// D'abords en WRITE afin de transmettre le registre 
	// que l'on veut lire ainsi que l'adresse du slave
	//-----------------------------------------------------------------------------
		
	// Début de la transmission --> mode write et START condition 
  iI2C_TxRxSelect(kTxMode); 
  iI2C_SetStartState();
  
  // Transmission de l'adresse en WRITE du slave dans le registre de données
	// --> obligatoire protocolle I2C, le slave doit d'abords répondre à son adresse
	iI2C_SendData(kMpl3115A2WriteAdr);
	
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	if(aNoAck==true)
		{
			// FIN de la lecture
			iI2C_TxRxSelect(kTxMode);
			iI2C_SetAckMode(kNoAck);
			iI2C_SetStopState();
			return false;
		}
	
	
	// Transmission de l'adresse du registre que l'on veut écrire
	iI2C_SendData(aReg);
		
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	if(aNoAck==true)
		{
			// FIN de la lecture
			iI2C_TxRxSelect(kTxMode);
			iI2C_SetAckMode(kNoAck);
			iI2C_SetStopState();
			return false;
		}


	// Transmission de la donnée que l'on veut écrire dans le registre
	iI2C_SendData(aVal);
		
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	// FIN de la lecture
	iI2C_TxRxSelect(kTxMode);
	iI2C_SetAckMode(kNoAck);
	iI2C_SetStopState();
	if(aNoAck==true)
		{
			return false;
		}
	else
		{
			return true;
		}		
}

//-----------------------------------------------------------------------------
// Lecture de l'altitude (MPL3115A2)
// *aAltitude: adresse de la variable contenant la mesure de l'alitude en m
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetAlt(float *aAltitude)
{
	UInt8 aStatus;
	UInt8 aVal;
	bool aRet;
	UInt32 aAlt;

	// Configuration du mode actif et lecture de l'altitude, delta T 6ms
	aRet=mMeteo_SetData(kCTRL_REG1,0x81);
		
	// Attend que la mesure soit disponible
	do
		{
			aRet=mMeteo_GetData(kDR_STATUS,&aStatus);
		}
	while((aStatus&kNewAltorPress)==0x00);
	
	// Lecture et mise en forme du résultat
	aRet=mMeteo_GetData(kOUT_P_MSB,&aVal);
	aAlt=(aVal<<24);
	aRet=mMeteo_GetData(kOUT_P_CSB,&aVal);
	aAlt|=(aVal<<16);
	aRet=mMeteo_GetData(kOUT_P_LSB,&aVal);
	aAlt|=(aVal<<8);
	
	*aAltitude=aAlt;
	
	// Passage en mode standby
	aRet=mMeteo_SetData(kCTRL_REG1,0x80);
		
	return aRet;	
}

//-----------------------------------------------------------------------------
// Lecture de la pression (MPL3115A2)
// *aPressure: 	adresse de la variable contenant la mesure de la pression en pascal
// retour : 		true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetPressure(float *aPressure)
{
	UInt8 aStatus;
	UInt8 aVal;
	bool aRet;
	UInt32 aPres;
	
	// Configuration du mode actif et lecture de la pression, delta T 6ms
	aRet=mMeteo_SetData(kCTRL_REG1,0x01);
		
	// Attend que la mesure soit disponible
	do
		{
			aRet=mMeteo_GetData(kDR_STATUS,&aStatus);
		}
	while((aStatus&kNewAltorPress)==0x00);
	
	// Lecture et mise en forme du résultat
	aRet=mMeteo_GetData(kOUT_P_MSB,&aVal);
	aPres=(aVal<<16);
	aRet=mMeteo_GetData(kOUT_P_CSB,&aVal);
	aPres|=(aVal<<8);
	aRet=mMeteo_GetData(kOUT_P_LSB,&aVal);
	aPres|=aVal;
	
	*aPressure=aPres;
	
	// Passage en mode standby
	aRet=mMeteo_SetData(kCTRL_REG1,0x00);
	
	return aRet;	
}

//-----------------------------------------------------------------------------
// Lecture de la température (MPL3115A2)
// *aTemperature: 	adresse de la variable contenant la mesure de la température
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetTemp(float *aTemperature)
{
	UInt8 aStatus;
	UInt8 aVal;
	bool aRet;
	UInt32 aTemp;

	// Attend que la mesure soit disponible
	do
		{
			aRet=mMeteo_GetData(kDR_STATUS,&aStatus);
		}
	while((aStatus&kNewTemp)==0x00);
	
	// Lecture et mise en forme du résultat
	aRet=mMeteo_GetData(kOUT_T_MSB,&aVal);
	aTemp=(aVal<<8);
	aRet=mMeteo_GetData(kOUT_T_LSB,&aVal);
	aTemp|=aVal;
	
	*aTemperature=aTemp;
	
	return aRet;
}


//-----------------------------------------------------------------------------
// Lecture d'un registre du capteur HTU21 (humidité et temp)
// aReg:    le registre que l'on veut lire
// aData:   l'adresse du variable dans laquelle on écrit le contenu du registre
// aCrc:		l'adresse du variable dans laquelle on écrit le CRC
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
static bool mMeteo_HTU21GetData(HTU21RegisterEnum aReg, UInt16* aData, UInt8* aCrc)
{
	bool aNoAck=false;
		
	// Enable transmit ACK
  iI2C_SetAckMode(kAckAuto);

  // Attend que le bus soit libre
  while(true==iI2C_ReadStatus(kBUSY)); 
	
	//-----------------------------------------------------------------------------
	// D'abords en WRITE afin de transmettre le registre 
	// que l'on veut lire ainsi que l'adresse du slave
	//-----------------------------------------------------------------------------
		
	// Début de la transmission --> mode write et START condition 
  iI2C_TxRxSelect(kTxMode); 
  iI2C_SetStartState();
  
  // Transmission de l'adresse en WRITE du slave dans le registre de données
	// --> obligatoire protocolle I2C, le slave doit d'abords répondre à son adresse
	iI2C_SendData(kHTU21WriteAdr);

	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	if(aNoAck==true)
		{
			// FIN de la lecture
			iI2C_TxRxSelect(kTxMode);
			iI2C_SetAckMode(kNoAck);
			iI2C_SetStopState();
			return false;
		}
	
	// Transmission de l'adresse du registre que l'on veut lire
	iI2C_SendData(aReg);
	
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Est-ce que le slave est content --> Read Ack
	aNoAck=iI2C_ReadStatus(kRxAK);
	
	if(aNoAck==true)
		{
			// FIN de la lecture
			iI2C_TxRxSelect(kTxMode);
			iI2C_SetAckMode(kNoAck);
			iI2C_SetStopState();
			return false;
		}
			
	
	//-----------------------------------------------------------------------------
	// Passage en READ
	//-----------------------------------------------------------------------------
	
	// Transmission de l'adresse en READ du slave dans le registre de données
	// --> obligatoire protocolle I2C, le slave doit d'abords répondre à son adresse
	// --> Attend le ACK --> fin de la mesure
	do
		{
			// Nouvelle condition START 
			iI2C_SetRepeatedStartSate();
				
			iI2C_SendData(kHTU21ReadAdr);
	
			// Attend la fin de la transmission
			iI2C_WaitEndOfRxOrTx();
	
			// Est-ce que le slave est content --> Read Ack
			aNoAck=iI2C_ReadStatus(kRxAK);
		}
	while(aNoAck==true);
	
	
	// Passage en mode READ (Le MSB de la mesure a déjà été reçu 
	// --> En lecture c'est au master d'envoyer le ACK
	iI2C_SetAckMode(kAckAuto);
	iI2C_TxRxSelect(kRxMode); 
	
	// Lecture du MSB de la mesure et transmission du clock pour le LSB
	*aData=iI2C_ReadData()<<8;
	
	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// Disable transmit ACK
	iI2C_SetAckMode(kNoAck);
		
	// Lecture du LSB de la mesure et transmission du clock pour le CRC
	*aData|=iI2C_ReadData();

	// Attend la fin de la transmission
	iI2C_WaitEndOfRxOrTx();
	
	// FIN de la lecture
	// Génération de la condition STOP
	// Obligatoire de faire le STOP avant la lecture pour 
	// ne pas émettre de clock à nouveau!
	iI2C_TxRxSelect(kTxMode);
	iI2C_SetAckMode(kNoAck);
	iI2C_SetStopState();
	
	// Lecture du CRC
	*aCrc=iI2C_ReadData();
			
	return true;	
}

//-----------------------------------------------------------------------------
// Lecture de l'humidité (HTU21D)
// *aHumidite: 	adresse de la variable contenant la mesure de l'Humidité
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetHumidite(UInt16 *aHumidite)
{
	bool aRet;
	UInt8 aCrc;

	aRet=mMeteo_HTU21GetData(kHTU21Hum,aHumidite,&aCrc);
	
	return aRet;
}

//-----------------------------------------------------------------------------
// Lecture de la température du capteur d'humidité (HTU21D)
// *aTemp: 	adresse de la variable contenant la mesure de la température
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetHumiditeTemp(UInt16 *aTemp)
{
	bool aRet;
	UInt8 aCrc;

	aRet=mMeteo_HTU21GetData(kHTU21Temp,aTemp,&aCrc);
	    			 
	return aRet;
}
