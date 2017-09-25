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
                    continue;
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
    dbgOutputLoc(ISR_MOTOR_L_START);
    dbgOutputVal(distL);
    
    // remove 1 cm from the remaining distance to go
    distL--; 
    // Is the current task complete? 
    if (distL <= 0)
    {
        dbgOutputLoc(ISR_MOTOR_L_DONE);
        // if we're done, set the speed to zero
        setMotorL_DC(0);
        // look for next task in queue
        if (!leftQIsEmpty())
        {
            dbgOutputLoc(ISR_MOTOR_L_Q_READ);
            // read data
            struct pwmQueueData data;
            motorL_recvQInISR(&data);
            
            dbgOutputVal(data.dist);
            // set motor motion stuff
            setMotorL_DC(data.dc);
            distL = data.dist;
            if (data.dir == FORWARD)
                setMotorL_Fwd();
            else
                setMotorL_Bck();
        }
    }
    // else continue the current task, so do nothing here
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}

// motor R
void IntHandlerDrvTmrInstance1(void)
{
    dbgOutputLoc(ISR_MOTOR_R_START);
    
    // remove 1 cm from the remaining distance to go
    distR--; 
    // Is the current task complete? 
    if (distR <= 0)
    {
        dbgOutputLoc(ISR_MOTOR_R_DONE);
        // if we're done, set the speed to zero
        setMotorR_DC(0);
        // look for next task in queue
        if (!rightQIsEmpty())
        {
            dbgOutputLoc(ISR_MOTOR_R_Q_READ);
            // read data
            struct pwmQueueData data;
            motorR_recvQInISR(&data);
            
            dbgOutputVal(data.dist);
            // set motor motion stuff
            setMotorR_DC(data.dc);
            distR = data.dist;
            if (data.dir == FORWARD)
                setMotorR_Fwd();
            else
                setMotorR_Bck();
        }
    }
    // else continue the current task, so do nothing here
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}

void IntHandlerDrvTmrInstance2(void)
{
    isr_count++;
    struct navQueueData data;
    // send speed and direction at rate of 2 Hz
    if (isr_count % 5 == 0)
    {
        data.type = SPEEDS;
        data.a = (LATC & MOTOR_RIGHT_DIR_PIN);
        data.b = (LATG & MOTOR_LEFT_DIR_PIN);
        data.c = getMotorR_DC();
        data.d = getMotorL_DC();
        
        sendMsgToNavQFromISR(data);
    }
    // send positions at 0.75 Hz
    if (isr_count % 15 == 0)
    {
        // TODO
        data.type = POSITION;
        data.a = 0; // x
        data.b = 0; // y
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}