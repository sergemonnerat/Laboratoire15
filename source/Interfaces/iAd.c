/*
------------------------------------------------------------
Copyright 2003-20xx Haute �cole ARC Ing�ni�rie, Switzerland. 
All rights reserved.
------------------------------------------------------------
Nom du fichier :	iAd.c
Auteur et Date :	Monnerat Serge 8.5.20xx

Description dans le fichier iAd.h
------------------------------------------------------------
*/


#include "iAd.h"
#include <MKL46Z4.h>

//------------------------------------------------------------
// Configuration du convertisseur AD
//------------------------------------------------------------
void iAd_Config(void)
{

	// ADC clock enable
	// System Clock Gating Control Register 6 (SIM_SCGC6)
	SIM->SCGC6|=SIM_SCGC6_ADC0_MASK;
	
	// Configuration du crossbar pour les pin du convertisseur ADC
	// Pin Control Register n (PORTx_PCRn) --> ADC = alternative 0
	PORTB->PCR[1]|=PORT_PCR_MUX(0);
	PORTB->PCR[2]|=PORT_PCR_MUX(0);
	PORTB->PCR[3]|=PORT_PCR_MUX(0);
	
	// ADC configuration register 1 (ADCx_CFG1)
	// ADLPC=0, Normal power configuration 
	// ADIV=1,The divide ratio is 2 and the clock rate is (input clock)/2 --> 24MHz/2 = 12MHz
	// ADLSMP=0,short sample time
	// MODE=01,It is single-ended 12-bit conversion
	// ADICLK=0,input clock: Bus clock divided by 1
	ADC0->CFG1=0;
	ADC0->CFG1|=(ADC_CFG1_ADIV(1)|ADC_CFG1_MODE(1)|ADC_CFG1_ADICLK(0));
	
	// Configuration register 2 (ADCx_CFG2)
	// ADACKEN=0,Asynchronous clock output disabled
	// ADHSC=1,Highspeed conversion sequence selected --> clock > 8MHz
	// ADLSTS=0,Default longest sample time (20 extra ADCK cycles; 24 ADCK cycles total) --> pas utilis�
	// AD clock = Bus clock / 2 = 24Mhz/2=12MHz
	// One channel conversion time = 1/12MHz * 22 = 1.833 us --> 545.55 ksamples/s
	ADC0->CFG2|=ADC_CFG2_ADHSC_MASK;
		
	// ADC status and control registers 1 (ADCx_SC1n)
	// AIEN=0,Conversion complete interrupt disabled
	// DIFF=0,Differential mode disable -> Single-ended conversions and input channels are selected
	// ADCH=0x1f,Input channel select -> Module disabled
	ADC0->SC1[0]=0;
	ADC0->SC1[0]|=(ADC_SC1_ADCH(0x1f));
	
	// Status and control register 2 (ADCx_SC2)
	// ADTRG=0,Software trigger selected
	ADC0->SC2=0;
	
	// Status and control register 2 (ADCx_SC3)
	// ADCO=0, One conversion or one set of conversions if the hardware average function is enabled (AVGE=1) after initiating a conversion.
	// AVGE=0, Hardware average function disabled
	// AVGS=0, 4 samples averaged
	ADC0->SC3=0;
}

//------------------------------------------------------------
// Lecture du r�sultat
// Retour: le r�sultat
//------------------------------------------------------------
UInt16 iAd_GetResult(void)
{
	UInt16 aVal;
	
	// Lecture du r�sultat
	aVal=ADC0->R[0];
	
	return aVal;
}

//------------------------------------------------------------
// S�lection du canal � convertir
// aCh: quel canal
//------------------------------------------------------------
void iAd_SelectChannel(ADCChannelEnum aCh)
{
	UInt16 aVal;
	
	// Reset du champ
	ADC0->SC1[0]&=(~ADC_SC1_ADCH_MASK);
	
	// Configuration du canal
	ADC0->SC1[0]|=(ADC_SC1_ADCH(aCh));
}


//------------------------------------------------------------
// Lecture du drapeau de fin de conversion
// Retour: �tat du flag
//------------------------------------------------------------
bool iAd_GetCompleteConvStatus(void)
{
	bool aRet;
	
	// Lecture du flag
	aRet=(ADC0->SC1[0]&ADC_SC1_COCO_MASK)==ADC_SC1_COCO_MASK;
	
	return aRet;
}

//------------------------------------------------------------
// Calibration du convertisseur AD
// Retour: true = OK, false = KO
//------------------------------------------------------------
bool iAd_Cal(void)
{
  UInt16 aCalVar=0;
  bool aRet;
  
  // Max ADC clock -> 4MHz
	ADC0->CFG1&=((~ADC_CFG1_ADIV_MASK)&(~ADC_CFG1_ADICLK_MASK));
	ADC0->CFG1|=(ADC_CFG1_ADIV(3)|ADC_CFG1_MODE(1)|ADC_CFG1_ADICLK(1)|ADC_CFG1_ADLSMP_MASK);
	
	// Enable Software Conversion Trigger for Calibration Process
	ADC0->SC2&=(~ADC_SC2_ADTRG_MASK);
	// Set single conversion, clear avgs bitfield for next writing
	ADC0->SC3&= ((~ADC_SC3_ADCO_MASK) & (~ADC_SC3_AVGS_MASK));
	// Turn averaging ON and set at max value ( 32 )
	ADC0->SC3|= (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3) );
	// Start CAL
	ADC0->SC3 |= ADC_SC3_CAL_MASK ;
	// Wait calibration end
	while ( (ADC0->SC1[0] & ADC_SC1_COCO_MASK ) == false );
	
	// Check for Calibration fail error and return
	if ((ADC0->SC3&ADC_SC3_CALF_MASK) == ADC_SC3_CALF_MASK)
		{  
			aRet=false;     
		}
	else
		{
			// Calculate plus-side calibration
			aCalVar = 0x00;
			
			aCalVar =  ADC0->CLP0;
			aCalVar += ADC0->CLP1;
			aCalVar += ADC0->CLP2;
			aCalVar += ADC0->CLP3;
			aCalVar += ADC0->CLP4;
			aCalVar += ADC0->CLPS;
			aCalVar = aCalVar/2;
			aCalVar |= 0x8000; // Set MSB
			ADC0->PG= ADC_PG_PG(aCalVar);
		
			// Calculate minus-side calibration
			aCalVar = 0x00;
		
			aCalVar =  ADC0->CLM0;
			aCalVar += ADC0->CLM1;
			aCalVar += ADC0->CLM2;
			aCalVar += ADC0->CLM3;
			aCalVar += ADC0->CLM4;
			aCalVar += ADC0->CLMS;
			aCalVar = aCalVar/2;
			aCalVar |= 0x8000; // Set MSB
			ADC0->MG = ADC_MG_MG(aCalVar);
			
			// Clear CAL bit
			ADC0->SC3&= (~ADC_SC3_CAL_MASK);
			
			// Return OK
			aRet=true;  
		}
  		
  return aRet;
}
