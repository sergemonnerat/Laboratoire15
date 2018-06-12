/*
------------------------------------------------------------
Copyright 2003-201x Haute école ARC Ingéniérie, Switzerland. 
All rights reserved.
------------------------------------------------------------
File name : 	def.h	
Author and date :	Monnerat Serge 06.06.2014

Goal : 

-----------------------------------------------------------------------------
History:
-----------------------------------------------------------------------------

$History: $

-----------------------------------------------------------------------------
*/

#ifndef DEF_H_
#define DEF_H_

//-----------------------------------------------------------------------------
// Déclaration des constantes
//-----------------------------------------------------------------------------
#define kPITTime 1.0

//-----------------------------------------------------------------------------
// Déclaration des types
//-----------------------------------------------------------------------------
typedef int bool;
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

typedef unsigned char				UInt8;  /*  8 bits */
typedef unsigned short int	UInt16; /* 16 bits */
typedef unsigned long int		UInt32; /* 32 bits */

typedef char			    			Int8;   /*  8 bits */
typedef short int	        	Int16;  /* 16 bits */
typedef int		            	Int32;  /* 32 bits */

typedef volatile Int8				VInt8;  /*  8 bits */
typedef volatile Int16			VInt16; /* 16 bits */
typedef volatile Int32			VInt32; /* 32 bits */

typedef volatile UInt8			VUInt8;  /*  8 bits */
typedef volatile UInt16			VUInt16; /* 16 bits */
typedef volatile UInt32			VUInt32; /* 32 bits */

//-----------------------------------------------------------------------------
// Déclaration des macros permettant l'enable/disable des interruptions
//-----------------------------------------------------------------------------
// Enable maskable interrupts
#define EnableInterrupts __asm(" CPSIE i");

// Disable maskable interrupts
#define DisableInterrupts __asm(" CPSID i");


#endif /* DEF_H_ */
