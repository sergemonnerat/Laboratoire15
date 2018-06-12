/*
 * mCpu.c
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */


#include "mCpu.h"
#include "iCpu.h"

//-----------------------------------------------------------------------
// Congiguration générale de la CPU
//-----------------------------------------------------------------------
void mCpu_Setup(void)
{
	// Configuration du clock de la CPU et du bus
	iCpu_CLKInit();
}

