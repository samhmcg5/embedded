/*******************************************************************************
 System Interrupt Source File

  File Name:
    sys_interrupt_a.S

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the 
    interrupt sub-system.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END
/*
*********************************************************************************************************
*                                           INCLUDES
*********************************************************************************************************
*/
#include <xc.h>



#include "ISR_Support.h"

 

/* TMR Instance 0 Interrupt */
   .extern  IntHandlerDrvTmrInstance0

   .section	.vector_12,code, keep
   .equ     __vector_dispatch_12, IntVectorDrvTmrInstance0
   .global  __vector_dispatch_12
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvTmrInstance0

IntVectorDrvTmrInstance0:
    la    $26,  _IntHandlerDrvTmrInstance0         
    jr    $26
    nop
	.end	IntVectorDrvTmrInstance0

   .section	.DrvTmrInstance0_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvTmrInstance0

_IntHandlerDrvTmrInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvTmrInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvTmrInstance0


/* TMR Instance 1 Interrupt */
   .extern  IntHandlerDrvTmrInstance1

   .section	.vector_16,code, keep
   .equ     __vector_dispatch_16, IntVectorDrvTmrInstance1
   .global  __vector_dispatch_16
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvTmrInstance1

IntVectorDrvTmrInstance1:
    la    $26,  _IntHandlerDrvTmrInstance1         
    jr    $26
    nop
	.end	IntVectorDrvTmrInstance1

   .section	.DrvTmrInstance1_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvTmrInstance1

_IntHandlerDrvTmrInstance1:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvTmrInstance1
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvTmrInstance1


/* TMR Instance 2 Interrupt */
   .extern  IntHandlerDrvTmrInstance2

   .section	.vector_20,code, keep
   .equ     __vector_dispatch_20, IntVectorDrvTmrInstance2
   .global  __vector_dispatch_20
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvTmrInstance2

IntVectorDrvTmrInstance2:
    la    $26,  _IntHandlerDrvTmrInstance2         
    jr    $26
    nop
	.end	IntVectorDrvTmrInstance2

   .section	.DrvTmrInstance2_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvTmrInstance2

_IntHandlerDrvTmrInstance2:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvTmrInstance2
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvTmrInstance2




/* USART Instance 0 Interrupt */

   .extern  IntHandlerDrvUsartInstance0

   .section	.vector_24,code, keep
   .equ     __vector_dispatch_24, IntVectorDrvUsartInstance0
   .global  __vector_dispatch_24
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvUsartInstance0

IntVectorDrvUsartInstance0:
    la    $26,  _IntHandlerDrvUsartInstance0         
    jr    $26
    nop
	.end	IntVectorDrvUsartInstance0

   .section	.DrvUsartInstance0_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvUsartInstance0

_IntHandlerDrvUsartInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvUsartInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvUsartInstance0



/* I2C Instance 0 Interrupt */

   .extern  IntHandlerDrvI2CInstance0

   .section	.vector_25,code, keep
   .equ     __vector_dispatch_25, IntVectorDrvI2CInstance0
   .global  __vector_dispatch_25
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvI2CInstance0

IntVectorDrvI2CInstance0:
    la    $26,  _IntHandlerDrvI2CInstance0         
    jr    $26
    nop
	.end	IntVectorDrvI2CInstance0

   .section	.DrvI2CInstance0_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvI2CInstance0

_IntHandlerDrvI2CInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvI2CInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvI2CInstance0



/* I2C Instance 1 Interrupt */

   .extern  IntHandlerDrvI2CInstance1

   .section	.vector_33,code, keep
   .equ     __vector_dispatch_33, IntVectorDrvI2CInstance1
   .global  __vector_dispatch_33
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvI2CInstance1

IntVectorDrvI2CInstance1:
    la    $26,  _IntHandlerDrvI2CInstance1         
    jr    $26
    nop
	.end	IntVectorDrvI2CInstance1

   .section	.DrvI2CInstance1_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvI2CInstance1

_IntHandlerDrvI2CInstance1:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvI2CInstance1
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvI2CInstance1


/*******************************************************************************
 End of File
 */

