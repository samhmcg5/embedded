#include "system/common/sys_common.h"
#include "communication.h"
#include "navigation.h"
#include "motor_globals.h"
#include "motor_control.h"
#include "system_definitions.h"
#include "nav_globals.h"

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

    /* Clear the counter registers */
    DRV_TMR0_CounterClear();
    DRV_TMR1_CounterClear();

    /* increment the distnace traveled per motor in ticks*/
    distL        += ticksL;
    distR        += ticksR;
    total_ticksL += ticksL;
    total_ticksR += ticksR;

    /* Are Both Motors Stopped ? */
    bool stopped  = getMotorL_DC() == 0 && getMotorR_DC() == 0;
    bool fwdORrev = getMotorL_Dir() == getMotorR_Dir();
    bool LorR     = getMotorL_Dir() != getMotorR_Dir();

    /* Outgoing Message Construction */
    // only update position if moving (dc != 0)
    struct navQueueData data;
    if (fwdORrev && distL >= TICKS_PER_CM && getMotorL_DC() != 0)
    {
        unsigned int cm = (distL / TICKS_PER_CM) - prev_cm;
        prev_cm = distL / TICKS_PER_CM;

        data.type = POS_UPDATE;
        data.a = cm;
        data.b = getMotorAction();
        sendMsgToNavQFromISR(data);
    }
    else if (getMotorAction() == TURN_LEFT && distL >= TP_DEGREE_L && getMotorL_DC != 0)
    {
        unsigned int deg = (distL / TP_DEGREE_L) - prev_cm;
        prev_cm = distL / TP_DEGREE_L;
        data.type = POS_UPDATE;
        data.a = deg;
        data.b = getMotorAction();
        sendMsgToNavQFromISR(data);
    }
    else if (getMotorAction() == TURN_RIGHT && distL >= TP_DEGREE_R && getMotorL_DC != 0)
    {
        unsigned int deg = (distL / TP_DEGREE_R) - prev_cm;
        prev_cm = distL / TP_DEGREE_R;
        data.type = POS_UPDATE;
        data.a = deg;
        data.b = getMotorAction();
        sendMsgToNavQFromISR(data);
    }

    if (distL >= 10*TICKS_PER_CM)
        integral = 0;

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

    /* If we are moving, try to correct the motion */
    if ( getMotorL_DC() != 0 && getMotorR_DC() != 0 )
    {
        // int offset = ( ticksL - ticksR ) * kp - 5;
        int error = ticksL - ticksR;
        integral  = integral + error;
        unsigned int new_dc;
        if (fwdORrev)
            new_dc = getMotorR_DC() + (error * KP) + (integral * KI) - (getMotorR_DC() / 75);
        else
            new_dc = getMotorR_DC() + (error * KP) + (integral * KI);

        if (new_dc > getMotorL_DC())
            new_dc = getMotorL_DC();

        setMotorR_DC( new_dc );
    }

    // magnet stuff, rate = 2 Hz
    if (isr_count % 5 == 0)
    {
        // tell server what the current magnet state should be 
        data.type = MAG_REQUEST;
    }
    // get speed data, rate = 1 Hz
    if (isr_count % 10 == 0)
    {
        // speed
        data.type = SPEEDS;
        data.a = getMotorR_Dir();
        data.b = getMotorL_Dir();
        data.c = total_ticksR;
        data.d = total_ticksL;
        sendMsgToNavQFromISR(data);

        // position to server
        data.type = POSITION;
        data.a = posX; // x
        data.b = posY; // y
        data.c = orientation;
        sendMsgToNavQFromISR(data);

        isr_count = 0;

        total_ticksL = 0;
        total_ticksR = 0;
    }

    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}