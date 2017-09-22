#include "system/common/sys_common.h"
#include "communication.h"
#include "navigation.h"
#include "motor_control.h"
#include "motor_globals.h"
#include "system_definitions.h"

#define ADC_NUM_SAMPLES     16
#define SERVER_TIMEOUT      8

unsigned char stopCountR = 0;
unsigned char stopCountL = 0;

unsigned char distR = 0;
unsigned char distL = 0;

void IntHandlerDrvUsartInstance0(void)
{
    if (SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_RECEIVE)) 
    {
        if (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)) 
        {
            readUartReceived();//receive uart
        }
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        //SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_RECEIVE);
    }
    if (SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_TRANSMIT)) 
    {
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

    if (SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_ERROR)) {
        halt(DBG_ERROR_UART_ERROR_FLAG);
    }    
}
 
// motor L
void IntHandlerDrvTmrInstance0(void)
{   
    distL--;
    // if distance to go == 0, stop
//    if (distL <= 0)
//    {
//        setMotorL_DC(0);
//    }
    // check if new data is available
//    if (distL <= 0 && !leftQIsEmpty())
    if (!leftQIsEmpty())
    {
        struct pwmQueueData rec;
        motorL_recvQInISR(&rec);
        
//        if (rec.dir == FORWARD)
//            setMotorL_Fwd();
//        else
//            setMotorL_Bck();
        
        setMotorL_DC(rec.dc);
        distL = rec.dist;
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}

// motor R
void IntHandlerDrvTmrInstance1(void)
{
    distR--;
    // if distance to go == 0, stop
//    if (distR <= 0)
//    {
//        setMotorR_DC(0);
//    }
//    if (distR <= 0 && !rightQIsEmpty())
    if (!rightQIsEmpty())
    {
        struct pwmQueueData rec;
        motorL_recvQInISR(&rec);
        
//        if (rec.dir == FORWARD)
//            setMotorR_Fwd();
//        else
//            setMotorR_Bck();
        
        setMotorR_DC(rec.dc);
        distR = rec.dist;
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}

// 10 Hz timer interrupt, used for stops
void IntHandlerDrvTmrInstance2(void)
{
    unsigned char dcR = getMotorR_DC();
    unsigned char dcL = getMotorL_DC();
    
    if (dcR == 0)
        stopCountR++;
    if (dcL == 0)
        stopCountL++;
    
    // is stopped for more than 1 sec, trigger the interrupts
    if (stopCountR >= 10)
    {
        stopCountR = 0;
        //PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_4);
    }
    if (stopCountL >= 10)
    {
        stopCountL = 0;
        //PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_3);
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}
