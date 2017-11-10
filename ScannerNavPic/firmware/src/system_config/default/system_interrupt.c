#include "system/common/sys_common.h"
#include "communication.h"
#include "communication_globals.h"
#include "motor_control.h"
#include "motor_globals.h"
#include "system_definitions.h"
#include "line_sensor.h"

#define ADC_NUM_SAMPLES     16
#define SERVER_TIMEOUT      8

char isr_count = 0;
char isr_count1 = 0;

unsigned int sumArrayLen10(unsigned int * arr)
{
    unsigned int sum = 0;
    int i;
    for (i=0; i<10; i++)
        sum += arr[i];
    return sum;
}

void IntHandlerDrvI2CInstance0(void)
{
	DRV_I2C0_Tasks();
 
}

void IntHandlerDrvI2CInstance1(void)
{
	DRV_I2C1_Tasks();
 
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
    isr_count1++;

    /* Get the current value from the timer registers */
    short int ticksL = DRV_TMR0_CounterValueGet();
    short int ticksR = DRV_TMR1_CounterValueGet();

    /* Clear the counter registers */
    DRV_TMR0_CounterClear();
    DRV_TMR1_CounterClear();

    /* increment the distance traveled per motor in ticks*/
    distL        += ticksL;
    distR        += ticksR;
    total_ticksL += ticksL;
    total_ticksR += ticksR;

    /* Are Both Motors Stopped ? */
    bool stopped  = getMotorL_DC() == 0 && getMotorR_DC() == 0;
    bool fwdORrev = getMotorL_Dir() == getMotorR_Dir();
    bool LorR     = getMotorL_Dir() != getMotorR_Dir();

    /* Outgoing Message Construction */
    struct motorQueueData data;
    if (fwdORrev && distL >= TICKS_PER_CM && getMotorL_DC() != 0)
    {
        unsigned int cm = (distL / TICKS_PER_CM) - prev_cm;
        prev_cm = distL / TICKS_PER_CM;

        data.type = POS_UPDATE;
        data.dist = cm;
        data.action = getMotorAction();
        sendMsgToMotorQFromISR(data);
    }

    /* If we have reached the goal of ticks to travel ...  for either motor */
    if (distL >= goalL || distR >= goalR || stopped)
    {
        integral = 0;
        distL    = 0;
        distR    = 0;
        prev_cm  = 0;

        if (!leftQIsEmpty())
            readFromQandSetPins(LEFT);
        else
            setMotorL_DC(0);

        if (!rightQIsEmpty())
            readFromQandSetPins(RIGHT);
        else
            setMotorR_DC(0);
    }
 
    /* If we are moving forward, try to correct the motion */
    if ( getMotorR_DC() != 0 && (getMotorR_Dir() == 0) && (getMotorL_Dir() == 0) )
    {
        // int offset = ( ticksL - ticksR ) * kp - 5;
        int error = ( ticksR - ticksL );
        integral  = integral + error;
        unsigned int new_dc = getMotorL_DC() + (error * KP) + (integral * KI) - (getMotorL_DC() / 85);

        if (new_dc > getMotorR_DC())
            new_dc = getMotorR_DC();
        setMotorL_DC( new_dc );
    }
    
    /* If we are moving backward, try to correct the motion */
    if ( getMotorR_DC() != 0 && (getMotorR_Dir() == 1) && (getMotorL_Dir() == 1) )
    {
        // int offset = ( ticksL - ticksR ) * kp - 5;
        int error = ( ticksR - ticksL );
        integral  = integral + error;
        unsigned int new_dc = getMotorL_DC() + (error * KP) + (integral * KI) - (getMotorL_DC() / 70);

        if (new_dc > getMotorR_DC())
            new_dc = getMotorR_DC();
        setMotorL_DC( new_dc );
    }
    

    if (isr_count % 10 == 0)
    {
        // // position
        data.type = POSITION;
        data.dist = posX; // x
        sendMsgToMotorQFromISR(data);

        isr_count = 0;

        total_ticksL = 0;
        total_ticksR = 0;
    }

    // Sensor reading 
    struct sensorQueueData s_data;
    
    if(isr_count1 % 2 == 0)
    {
        s_data.x = '1';
        

        sendMsgToSensorQFromISR(s_data);
        isr_count1 = 0;
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}