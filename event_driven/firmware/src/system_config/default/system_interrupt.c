#include "system/common/sys_common.h"
#include "communication.h"
#include "system_definitions.h"

#define ADC_NUM_SAMPLES     16

// pull the front item off the outgoing uart queue

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
            Nop();
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
    unsigned char msg[UART_RX_QUEUE_SIZE];
    msg[0] == 'T';
    msg[1] == 'E';
    msg[2] == 'A';
    msg[3] == 'M';
    msg[4] == '_';
    msg[5] == '1';
    msg[6] == '4';
    msg[7] == '\0';
    commSendMsgFromISR(msg);
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
