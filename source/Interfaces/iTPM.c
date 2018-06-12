/*
------------------------------------------------------------
Copyright 2003-2012 Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name :	iTPM.c
Author and date :	Monnerat Serge 1 mars 2012

Description in the header (.h)

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#include "iTPM.h"
#include <MKL46Z4.h>
#include <core_cm0plus.h>

// Priorité de l'interruption TPM2
#define kTPM2Prio 2

// TPM2 overflow value
#define kTPM2OverflowValue 65535

static UInt16 sOldCh0Value;
static float sDeltaCh0=0;
static UInt16 sOldCh1Value;
static float sDeltaCh1=0;

//------------------------------------------------------------
// TPM setup
//------------------------------------------------------------
void iTPM_Config(void)
{
	// System Options Register 2 (SIM_SOPT2)
	// Choix de l'horloge source pour le TPM
	// Source du clock MCGPLLCLK/2, donc 24MHz
	SIM->SOPT2|=SIM_SOPT2_TPMSRC(1)|SIM_SOPT2_PLLFLLSEL_MASK;
	
	// Enable du clock du TPM
	// System Clock Gating Control Register 6 (SIM_SCGC6)
	SIM->SCGC6|=SIM_SCGC6_TPM0_MASK;	// Pwm
	SIM->SCGC6|=SIM_SCGC6_TPM2_MASK;	// IC
	
	// Configuration du crossbar pour les pin RX et TX (PTE16 et PTE17)
	// Pin Control Register n (PORTx_PCRn) --> TPM = alternative 3
	PORTE->PCR[30]|=PORT_PCR_MUX(3);	// Pwm
	PORTE->PCR[31]|=PORT_PCR_MUX(3);	// Pwm
	PORTE->PCR[22]|=PORT_PCR_MUX(3);	// IC
	PORTE->PCR[23]|=PORT_PCR_MUX(3);	// IC
	
	//--------------------------------------------------------------
	// TPM0 --> PWM
	//--------------------------------------------------------------
	// Status and Control (TPMx_SC)
	// Disables DMA transfers
	// Disable TOF interrupts. Use software polling or DMA request
	// TPM counter operates in up counting mode
	// TPM counter increments on every TPM counter clock
	// Prescale Factor Selection: Divide by 1 --> 24MHz/1=24MHz
	TPM0->SC=0|(TPM_SC_CMOD(1)|TPM_SC_PS(0));
	
	// Channel (n) Status and Control (TPMx_CnSC)
	// Edge-aligned PWM setup
	TPM0->CONTROLS[3].CnSC=0|(TPM_CnSC_ELSB_MASK|TPM_CnSC_MSB_MASK);
	TPM0->CONTROLS[4].CnSC=0|(TPM_CnSC_ELSB_MASK|TPM_CnSC_MSB_MASK);
	
	// Modulo (TPMx_MOD)
	// PWM period setup = 24MHz/(TPM0->MOD)--> TPM0->MOD= 24MHz/20kHz=1200
	TPM0->MOD=1200;
	
	//--------------------------------------------------------------
	// TPM2 --> IC
	//--------------------------------------------------------------
	// Enable de l'interruption UART2 Rx au niveau du NVIC
	// Le vecteur du UART2 est le numéro 14
	NVIC_EnableIRQ(TPM2_IRQn);
		
	// Configuration de la priorité de l'interruption PIT
	// O : plus haute priorité
	// 3 : plus basse priorité
	NVIC_SetPriority(TPM2_IRQn, kTPM2Prio);
		
	// Status and Control (TPMx_SC)
	// Disables DMA transfers
	// Disable TOF interrupts. Use software polling or DMA request
	// TPM counter operates in up counting mode
	// TPM counter increments on every TPM counter clock
	// Prescale Factor Selection: Divide by 1 --> 24MHz/1=24MHz
	TPM2->SC=0|(TPM_SC_CMOD(1)|TPM_SC_PS(0)|TPM_SC_TOIE_MASK);
	
	// Channel (n) Status and Control (TPMx_CnSC)
	// Rising edge capture et enable de l'interruption
	TPM2->CONTROLS[0].CnSC=0|(TPM_CnSC_ELSA_MASK|TPM_CnSC_CHIE_MASK);
	TPM2->CONTROLS[1].CnSC=0|(TPM_CnSC_ELSA_MASK|TPM_CnSC_CHIE_MASK);
	
	// Modulo (TPMx_MOD)
	// Valeur maximale, évite de trop fréquent overflow
	TPM2->MOD=kTPM2OverflowValue;
}

//------------------------------------------------------------
// Set PWM duty
//------------------------------------------------------------
void iTPM_UpdatePWM(PwmChannelEnum aChannel, float aDuty)
{
	if(aChannel==kPwmCh3)
		{
			TPM0->CONTROLS[3].CnV=TPM0->MOD*aDuty;
		}
	else if(aChannel==kPwmCh4) 
		{
			TPM0->CONTROLS[4].CnV=TPM0->MOD*aDuty;
		}
}

//------------------------------------------------------------
// Lecture du nb de step entre 2 flancs --> canal 0
//------------------------------------------------------------
float iTPM_GetFrequenceCh0(void)
{    
	float aVar;
	
	// Division par 0!
	if(sDeltaCh0>0)
		{
			// Lecture du nb de step entre 2 flancs --> canal 0
			aVar=(24E6/sDeltaCh0);
			// Encodeur de 16 pulse par tour
			aVar/=16.0;
			// Pour avoir des tr/mn
			aVar*=60.0;
			// Facteur de réduction du réducteur
			aVar/=18.75;
		}
	else
		{
			aVar=0;
		}
	
	return aVar;
}

//------------------------------------------------------------
// Lecture du nb de step entre 2 flancs --> canal 1
//------------------------------------------------------------
float iTPM_GetFrequenceCh1(void)
{    
	float aVar;
		
		// Division par 0!
		if(sDeltaCh0>0)
			{
				// Lecture du nb de step entre 2 flancs --> canal 0
				aVar=(24E6/sDeltaCh1);
				// Encodeur de 16 pulse par tour
				aVar/=16.0;
				// Pour avoir des tr/mn
				aVar*=60.0;
				// Facteur de réduction du réducteur
				aVar/=18.75;
			}
		else
			{
				aVar=0;
			}
		
		return aVar;
}

//------------------------------------------------------------
// Interruption TPM2
//------------------------------------------------------------
void TPM2_IRQHandler(void)
{
	UInt16 aVal=0;
	static UInt16 sOverflowCounter=0;
	
	// Contrôle si l'interruption est due à l'overflow
	if((TPM2->SC&TPM_SC_TOF_MASK)==TPM_SC_TOF_MASK)
		{
			// Reset du flag
			TPM2->SC|=TPM_SC_TOF_MASK;
			sOverflowCounter++;
		}
	
	// Contrôle si l'interruption est due à une capture
	if((TPM2->CONTROLS[0].CnSC&TPM_CnSC_CHF_MASK)==TPM_CnSC_CHF_MASK)
		{
			// Reset du flag
			TPM2->CONTROLS[0].CnSC|=TPM_CnSC_CHF_MASK;
			
			aVal=TPM2->CONTROLS[0].CnV;
			
			// Calcul du delta
			if(aVal>sOldCh0Value)
				sDeltaCh0=aVal-sOldCh0Value;
			else
				sDeltaCh0=aVal+(kTPM2OverflowValue-sOldCh0Value);
			
			// On ajoute les overflow --> très basse vitesse
			sDeltaCh0+=(sOverflowCounter*kTPM2OverflowValue);
			sOverflowCounter=0;
			
			// On sauve l'ancienne valeur du compteur
			sOldCh0Value=aVal;
		}
	
	// Contrôle si l'interruption est due à une capture
	if((TPM2->CONTROLS[1].CnSC&TPM_CnSC_CHF_MASK)==TPM_CnSC_CHF_MASK)
		{
			// Reset du flag
			TPM2->CONTROLS[1].CnSC|=TPM_CnSC_CHF_MASK;
			
			aVal=TPM2->CONTROLS[1].CnV;
			
			// Calcul du delta
			if(aVal>sOldCh1Value)
				sDeltaCh1=aVal-sOldCh1Value;
			else
				sDeltaCh1=aVal+(kTPM2OverflowValue-sOldCh1Value);
			
			// On ajoute les overflow --> très basse vitesse
			sDeltaCh1+=(sOverflowCounter*kTPM2OverflowValue);
			//sOverflowCounter=0;
			
			// On sauve l'ancienne valeur du compteur
			sOldCh1Value=aVal;
		}
}
