#include "peripherals.h"
#include "peripherals_globals.h"
#include "queue.h"
#include "portmacro.h"
#include "driver/tmr/drv_tmr_static.h"
#include "communication_globals.h"
#include <stdio.h>
#include <math.h>
PERIPHERALS_DATA peripheralsData;

void sendMsgToPeriphQ(struct periphQueueData msg){
    xQueueSendToBack(periph_q, &msg, portMAX_DELAY);
}
void sendMsgToPeriphQFromISR(struct periphQueueData msg){
    xQueueSendToBackFromISR(periph_q, &msg, NULL);
}
void setMagnet(bool on){
    if(on == MAGNET_STATE_ON){
        TRISFCLR = 0x0008;
        ODCFCLR  = 0x0008;
        
        LATFCLR = 0x0008;
        LATFSET = 0x0008;//RB11 is my pin
        magnet_state = 1;
    }
    else{
        TRISFCLR = 0x0008;
        ODCFCLR  = 0x0008;
        
        LATFCLR = 0x0008;
        magnet_state = 0;
    }
}

void setADC(bool on){
    if(on == ADC_STATE_ON){
        DRV_ADC_Open();
        adc_state = true;
    }
    else{
        DRV_ADC_Close();
        adc_state = false;
    }
}

void PERIPHERALS_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    peripheralsData.state = PERIPHERALS_STATE_INIT;
    setMagnet(MAGNET_STATE_OFF);
    objDist = 0;
    objHeld = false;
    periph_q = xQueueCreate(32, sizeof (struct periphQueueData));
}

void PERIPHERALS_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( peripheralsData.state )
    {
        /* Application's initial state. */
        case PERIPHERALS_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                peripheralsData.state = PERIPHERALS_STATE_SERVICE_TASKS;
            }
            break;
        }

        case PERIPHERALS_STATE_SERVICE_TASKS:
        {
            DRV_ADC_Open();
            DRV_TMR0_Start();
            struct periphQueueData recv;
            while(1){
                if(xQueueReceive(periph_q, &recv, portMAX_DELAY)){
                    dbgOutputLoc(PERIPH_START);
                    if(recv.type == COMMTASK_TO_PERIPHERAL_MSG){
                        //do something
                        dbgOutputLoc(PERIPH_COMM);
                        Nop();
                        if(recv.magnet == true){
                            setMagnet(MAGNET_STATE_ON);
                        }
                        else{
                            setMagnet(MAGNET_STATE_OFF);
                        }
                    }
                    else if(recv.type == ADC_TO_PERIPHERAL_MSG){
                        dbgOutputLoc(PERIPH_ADC);
                        Nop();
                        double convert = 2076.0/(recv.distance - 11.0);
                       objDist = convert;
                       if(objDist < 5.0)
                           objHeld = true;
                       else
                           objHeld = false;
                    }
                    else if(recv.type == TMR_TO_PERIPHERAL_MSG){
                        dbgOutputLoc(PERIPH_TMR);
                        Nop();
                        if(objDist != 0){
                            char buf[55];
                            sprintf(buf, STR_IR_DISTANCE, outgoing_seq, objDist, objHeld);
                            commSendMsgToUartQueue(buf);
                        }
                    }
                    else{
                        Nop();
                        halt(PERIPH_MSGQ_FAIL);
                    }
                }
            }
        }
        default:
        {
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
