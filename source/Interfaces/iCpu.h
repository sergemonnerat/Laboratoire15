/*
 * iCpu.h
 *
 *  Created on: Mar 21, 2014
 *      Author: serge.monnerat
 */


#ifndef __ICPU__
#define __ICPU__

#include "def.h"

//------------------------------------------------------------
// Core and peripheral clock init
// External clock = 8MHz
// Core clock = 48MHz
// Peripheral bus = 24MHz
// Flexbus = 24MHz
// Flash = 24MHz
//------------------------------------------------------------
void iCpu_CLKInit(void);

#endif
