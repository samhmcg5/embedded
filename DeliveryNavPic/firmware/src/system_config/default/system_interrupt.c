#include "system/common/sys_common.h"
#include "communication.h"
#include "navigation.h"
#include "motor_control.h"
#include "motor_globals.h"
#include "system_definitions.h"
#include "nav_globals.h"

#define ADC_NUM_SAMPLES     16
#define SERVER_TIMEOUT      8

char isr_count = 0;

void IntHandlerDrvUsartInstance0(void)
{
    if (SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_RECEIVE)) 
    {
        dbgOutputLoc(ISR_UART_RX);
        
        if (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)) 
        {
            readUartReceived();//receive uart
        }
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        //SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_RECEIVE);
    }
    if (SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_TRANSMIT)) 
    {
        dbgOutputLoc(ISR_UART_TX);
        
        while (1) 
        {
            if (!checkIfSendQueueIsEmpty()) 
            {
                unsigned char writeBuff;
                if (PLIB_USART_TransmitterBufferIsFull(USART_ID_1)) 
                {
                    break;
                }
                uartReceiveFromOutQueueInISR(&writeBuff);
                uartWriteMsg(writeBuff);
            } 
            else 
            {
                PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
                break;
            }
        }

        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
}
 
// motor L
void IntHandlerDrvTmrInstance0(void)
{   
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}

// motor R
void IntHandlerDrvTmrInstance1(void)
{
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}

void IntHandlerDrvTmrInstance2(void)
{
    isr_count++;
    /* Get the current value from the timer registers */
    short int ticksL = DRV_TMR0_CounterValueGet();
    short int ticksR = DRV_TMR1_CounterValueGet();
            
    total_ticksL += ticksL;
    total_ticksR += ticksR;
    
    /* increment the distnace traveled per motor in ticks*/
    distL        += ticksL;
    distR        += ticksR;
    
    /* Clear the counter registers */
    DRV_TMR0_CounterClear();
    DRV_TMR1_CounterClear();
    
    bool stopped = getMotorL_DC() == 0 && getMotorR_DC() == 0;
    /* If we have reached the goal of ticks to travel ...  for either motor */
    if (distL >= goalL || distR >= goalR || stopped)
    {
        distL = 0; // reset the distance traveled 
        if (!leftQIsEmpty())
            readFromQandSetPins(LEFT);
        else 
            setMotorL_DC(0);
        
        distR = 0;
        if (!rightQIsEmpty())
            readFromQandSetPins(RIGHT);
        else
            setMotorR_DC(0);
    }

//    if (distL < goalL && distR < goalR)
    if (getMotorL_DC() != 0 && getMotorR_DC() != 0)
    {
        // use the left motor as the 'master motor' and right as the slave
        float err = ticksL - ticksR;
        float kp = 0.0;
        
        if (getMotorAction() == FORWARD)
            kp = 2.5;
        else if (getMotorAction() == REVERSE)
            kp = 3.5;
        
        float offset = kp*err;
        short int new_dc = getMotorR_DC() + (int)offset;
        
        setMotorR_DC( new_dc );
    }
    
    struct navQueueData data;
    // get speed data, rate = 1 Hz
    if (isr_count % 20 == 0 && !stopped)
    {
        data.type = SPEEDS;
        data.a = getMotorR_Dir();
        data.b = getMotorL_Dir();
        data.c = total_ticksR;
        data.d = total_ticksL;
        
        sendMsgToNavQFromISR(data);
        
        total_ticksL = 0;
        total_ticksR = 0;
    }
    // rate = 0.5 Hz
    if (isr_count % 40 == 0)
    {
        // TODO
        data.type = POSITION;
        data.a = 0; // x
        data.b = 0; // y
        sendMsgToNavQFromISR(data);
        
        isr_count = 0;
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}