/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
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

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/common/sys_common.h"
#include "communication.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************

unsigned int voltage;

void IntHandlerDrvAdc(void)
{
    //voltage = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 0);
    /* Clear ADC Interrupt Flag */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_ADC_1);
    PLIB_ADC_SampleAutoStartEnable(ADC_ID_1); 
}


void IntHandlerDrvTmrInstance0(void)
{
    dbgOutputLoc(DBG_LOCATION_TMRTASK_START);
    voltage = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 2);
    PLIB_ADC_SampleAutoStartEnable(ADC_ID_1);
    sendMsgToQFromISR(voltage);
    
//    switch(tmr_message.msg_state) {
//        case TMR_MESSAGE_STATE_T:
//        {
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_BEFORE_SEND);
//            sendMsgToQFromISR('T');
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_AFTER_SEND);
//            tmr_message.msg_state = TMR_MESSAGE_STATE_E;
//            break;
//        }
//        case TMR_MESSAGE_STATE_E:
//        {
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_BEFORE_SEND);
//            sendMsgToQFromISR('E');
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_AFTER_SEND);
//            tmr_message.msg_state = TMR_MESSAGE_STATE_A;
//            break;
//        }
//        case TMR_MESSAGE_STATE_A:
//        {
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_BEFORE_SEND);
//            sendMsgToQFromISR('A');
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_AFTER_SEND);
//            tmr_message.msg_state = TMR_MESSAGE_STATE_M;
//            break;
//        }
//        case TMR_MESSAGE_STATE_M:
//        {            
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_BEFORE_SEND);
//            sendMsgToQFromISR('M');
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_AFTER_SEND);
//            tmr_message.msg_state = TMR_MESSAGE_STATE_1;
//            break;
//        }
//        case TMR_MESSAGE_STATE_1:
//        {
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_BEFORE_SEND);
//            sendMsgToQFromISR('1');
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_AFTER_SEND);
//            tmr_message.msg_state = TMR_MESSAGE_STATE_4;
//            break;
//        }
//        case TMR_MESSAGE_STATE_4:
//        {
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_BEFORE_SEND);
//            sendMsgToQFromISR('4');
//            dbgOutputLoc(DBG_LOCATION_TMRTASK_AFTER_SEND);
//            tmr_message.msg_state = TMR_MESSAGE_STATE_T;
//            break;
//        }
//        default:
//        {
//            break;
//        }
//    }
    
    dbgOutputLoc(DBG_LOCATION_TMRTASK_END);
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
}
 /*******************************************************************************
 End of File
*/
