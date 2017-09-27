#include "system/common/sys_common.h"
#include "communication.h"
#include "motor_control.h"
#include "motor_globals.h"
#include "system_definitions.h"
#include "line_sensor.h"

#define ADC_NUM_SAMPLES     16
#define SERVER_TIMEOUT      8

char isr_count = 0;

unsigned int sumArrayLen10(unsigned int * arr)
{
    unsigned int sum = 0;
    int i;
    for (i=0; i<10; i++)
        sum += arr[i];
    return sum;
}

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
    dbgOutputLoc(ISR_MOTOR_L_START);
    dbgOutputVal(distL);
    
    // remove 1 cm from the remaining distance to go
//    distL--; 
//    // Is the current task complete? 
//    if (distL <= 0)
//    {
//        dbgOutputLoc(ISR_MOTOR_L_DONE);
//        // if we're done, set the speed to zero
//        setMotorL_DC(0);
//        // look for next task in queue
//        if (!leftQIsEmpty())
//        {
//            dbgOutputLoc(ISR_MOTOR_L_Q_READ);
//            // read data
//            struct pwmQueueData data;
//            motorL_recvQInISR(&data);
//            
//            dbgOutputVal(data.dist);
//            // set motor motion stuff
//            setMotorL_DC(data.dc);
//            distL = data.dist;
//            if (data.dir == FORWARD)
//                setMotorL_Fwd();
//            else
//                setMotorL_Bck();
//        }
//    }
    // else continue the current task, so do nothing here
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}

// motor R
void IntHandlerDrvTmrInstance1(void)
{
    dbgOutputLoc(ISR_MOTOR_R_START);
    
    // remove 1 cm from the remaining distance to go
//    distR--; 
//    // Is the current task complete? 
//    if (distR <= 0)
//    {
//        dbgOutputLoc(ISR_MOTOR_R_DONE);
//        // if we're done, set the speed to zero
//        setMotorR_DC(0);
//        // look for next task in queue
//        if (!rightQIsEmpty())
//        {
//            dbgOutputLoc(ISR_MOTOR_R_Q_READ);
//            // read data
//            struct pwmQueueData data;
//            motorR_recvQInISR(&data);
//            
//            dbgOutputVal(data.dist);
//            // set motor motion stuff
//            setMotorR_DC(data.dc);
//            distR = data.dist;
//            if (data.dir == FORWARD)
//                setMotorR_Fwd();
//            else
//                setMotorR_Bck();
//        }
//    }
    // else continue the current task, so do nothing here
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}

void IntHandlerDrvTmrInstance2(void)
{
    isr_count++;
    /* Get the current value from the timer registers */
    short int ticksL = DRV_TMR0_CounterValueGet();
    short int ticksR = DRV_TMR1_CounterValueGet();
            
    /* Add to total ticks per this half second run */
//    total_ticksL[isr_count % 10] = ticksL;
//    total_ticksR[isr_count % 10] = ticksR;
    total_ticksL += ticksL;
    total_ticksR += ticksR;
    
    /* increment the distnace traveled per motor in ticks*/
    distL        += ticksL;
    distR        += ticksR;
    
    /* Clear the counter registers */
    DRV_TMR0_CounterClear();
    DRV_TMR1_CounterClear();
    
    /* If we have reached the goal of ticks to travel ... */
    if (distL >= goalL)
    {
        distL = 0; // reset the distance traveled 
        if (!leftQIsEmpty())
        {
            /* Read the next action off the queue */
            dbgOutputLoc(ISR_MOTOR_L_Q_READ);
            // read data
            struct pwmQueueData data;
            motorL_recvQInISR(&data);
            
            dbgOutputVal(data.dist);
            // set motor motion stuff
            setMotorL_DC(data.dc);
            goalL = data.dist * 60;
            if (data.dir == FORWARD)
                setMotorL_Fwd();
            else
                setMotorL_Bck();
        }
        else
        {
            /* Stop the motors if no task available */
            setMotorL_DC(0);
        }
    }
    if (distR >= goalL)
    {
        distR = 0;
        if (!rightQIsEmpty())
        {
            dbgOutputLoc(ISR_MOTOR_R_Q_READ);
            // read data
            struct pwmQueueData data;
            motorR_recvQInISR(&data);
            
            dbgOutputVal(data.dist);
            // set motor motion stuff
            setMotorR_DC(data.dc);
            goalR = data.dist * 60;
            if (data.dir == FORWARD)
                setMotorR_Fwd();
            else
                setMotorR_Bck();
        }
        else
        {
            setMotorR_DC(0);
        }
    }
    
//    struct navQueueData data;
//    // get speed data, rate = 1 Hz
//    if (isr_count % 10 == 0)
//    {
//        data.type = SPEEDS;
//        data.a = getMotorR_Dir();
//        data.b = getMotorL_Dir();
////        data.c = ticksR;
//        data.c = total_ticksR;
////        data.d = ticksL;
//        data.d = total_ticksL;
//        
//        sendMsgToNavQFromISR(data);
//        
//        total_ticksL = 0;
//        total_ticksR = 0;
//    }
//    // rate = 0.4 Hz
//    if (isr_count % 20 == 0)
//    {
//        // TODO
//        data.type = POSITION;
//        data.a = 0; // x
//        data.b = 0; // y
//        sendMsgToNavQFromISR(data);
//        
//        isr_count = 0;
//    }
    
    struct motorQueueData p_data;
    // rate = 0.4 Hz
    if (isr_count % 10 == 0)
    {
        // TODO
        p_data.type = POSITION;
        p_data.action = 0; 
        p_data.dist = 0;
        p_data.speed = 0;
        sendMsgToMotorQFromISR(p_data);
        
        //isr_count = 0;
    }
    
    struct sensorQueueData s_data;
    if(isr_count % 50 == 0)
    {
         // TODO
        s_data.front_sensor = 10;
        s_data.rear_sensor = 20;
        sendMsgToSensorQFromISR(s_data);
        
        isr_count = 0;
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}