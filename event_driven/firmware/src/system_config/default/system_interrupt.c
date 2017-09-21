#include "system/common/sys_common.h"
#include "communication.h"
#include "system_definitions.h"

#define ADC_NUM_SAMPLES     16
#define SERVER_TIMEOUT      8

COMMUNICATION_STATES global_state = COMMUNICATION_STATE_INIT;
int timerCount = 0;


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
 
void IntHandlerDrvTmrInstance0(void)
{ 
    COMMUNICATION_STATES state;
    uartReceiveFromTimeoutQInISR(&state);
    
    if (state != 0)
        global_state = state;
    
    if (global_state == COMM_AWAIT_RESPONSE)
        timerCount++;
    else
        timerCount = 0;
    
    if (timerCount > SERVER_TIMEOUT)
    {
        char buf[UART_RX_QUEUE_SIZE];
        buf[0] = '?';
        buf[1] = '\0';
        commSendMsgFromISR(buf);
        timerCount = 0;
    }
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
