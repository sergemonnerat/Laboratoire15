/*
------------------------------------------------------------
Copyright 2003-20xx Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier : 	mMeteo.h	
Auteur et Date :	Monnerat Serge 8.5.20xx

But : Module donnant accès aux capteurs météo 

Modifications
Date		Faite	Ctrl		Description
------------------------------------------------------------
*/

#ifndef __MMETEO__
#define __MMETEO__

#include"def.h"

// Enumération des registres du capteur de presion et de température MPL3115A2
typedef enum
{
	kSTATUS     = 0x00,
	kOUT_P_MSB  = 0x01,
	kOUT_P_CSB  = 0x02,
	kOUT_P_LSB  = 0x03,
	kOUT_T_MSB  = 0x04,
	kOUT_T_LSB  = 0x05,
	kDR_STATUS  = 0x06,
	kOUT_P_DELTA_MSB  = 0x07,
	kOUT_P_DELTA_CSB  = 0x08,
	kOUT_P_DELTA_LSB  = 0x09,
	kOUT_T_DELTA_MSB  = 0x0A,
	kOUT_T_DELTA_LSB  = 0x0B,
	kWHO_AM_I   = 0x0C,
	kF_STATUS   = 0x0D,
	kF_DATA     = 0x0E,
	kF_SETUP    = 0x0F,
	kTIME_DLY   = 0x10,
	kSYSMOD     = 0x11,
	kINT_SOURCE = 0x12,
	kPT_DATA_CFG = 0x13,
	kBAR_IN_MSB = 0x14,
	kBAR_IN_LSB = 0x15,
	kP_TGT_MSB  = 0x16,
	kP_TGT_LSB  = 0x17,
	kT_TGT      = 0x18,
	kP_WND_MSB  = 0x19,
	kP_WND_LSB  = 0x1A,
	kT_WND      = 0x1B,
	kP_MIN_MSB  = 0x1C,
	kP_MIN_CSB  = 0x1D,
	kP_MIN_LSB  = 0x1E,
	kT_MIN_MSB  = 0x1F,
	kT_MIN_LSB  = 0x20,
	kP_MAX_MSB  = 0x21,
	kP_MAX_CSB  = 0x22,
	kP_MAX_LSB  = 0x23,
	kT_MAX_MSB  = 0x24,
	kT_MAX_LSB  = 0x25,
	kCTRL_REG1  = 0x26,
	kCTRL_REG2  = 0x27,
	kCTRL_REG3  = 0x28,
	kCTRL_REG4  = 0x29,
	kCTRL_REG5  = 0x2A,
	kOFF_P      = 0x2B,
	kOFF_T      = 0x2C,
	kOFF_H      = 0x2D
}MeteoRegisterEnum;


// Enumération des registres du capteur d'humidité et de température HTU21D
typedef enum
{
	kHTU21Temp=0xF3,
	kHTU21Hum=0xF5,
	kHTU21Reset=0xFE
}HTU21RegisterEnum;

//-----------------------------------------------------------------------------
// Configuration du module      
//-----------------------------------------------------------------------------
void mMeteo_Setup(void);

//-----------------------------------------------------------------------------
// Ouverture du module      
//-----------------------------------------------------------------------------
void mMeteo_Open(void);

//-----------------------------------------------------------------------------
// Fermeture du module      
//-----------------------------------------------------------------------------
void mMeteo_Close(void);

//-----------------------------------------------------------------------------
// Lecture de l'altitude (MPL3115A2)
// *aAltitude: 	adresse de la variable contenant la mesure de l'alitude en m
// retour : 		true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetAlt(float *aAltitude);

//-----------------------------------------------------------------------------
// Lecture de la pression (MPL3115A2)
// *aPressure: 	adresse de la variable contenant la mesure de la pression en pascal
// retour : 		true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetPressure(float *aPressure);

//-----------------------------------------------------------------------------
// Lecture de la température (MPL3115A2)
// *aTemperature: 	adresse de la variable contenant la mesure de la température
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetTemp(float *aTemperature);

//-----------------------------------------------------------------------------
// Lecture de l'humidité (HTU21D)
// *aHumidite: 	adresse de la variable contenant la mesure de l'Humidité
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetHumidite(UInt16 *aHumidite);

//-----------------------------------------------------------------------------
// Lecture de la température du capteur d'humidité (HTU21D)
// *aTemp: 	adresse de la variable contenant la mesure de la température
// retour : true --> lecture OK, false --> lecture KO
//-----------------------------------------------------------------------------
bool mMeteo_GetHumiditeTemp(UInt16 *aTemp);

#endif
