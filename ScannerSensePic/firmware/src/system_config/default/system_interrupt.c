#include "system/common/sys_common.h"
#include "communication.h"
#include "scanner.h"
#include "system_definitions.h"

#define ADC_NUM_SAMPLES     16
#define SERVER_TIMEOUT      8
unsigned int count = 0;

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
           
void IntHandlerDrvAdc(void)
{
    dbgOutputLoc(ADC_ISR_START);
    
    ADC_SAMPLE avg = 0;
    int i;
    for(i = 0; i < ADC_NUM_SAMPLES; i++){
        avg += PLIB_ADC_ResultGetByIndex(ADC_ID_1, i);
    }
    avg = avg/ADC_NUM_SAMPLES;
    
    struct scanQueueData data;
    data.type = ADC;
    data.dist = avg;
    
    data.color = 0;
    if (avg >= 600) {
        data.color = 0;
    }
    else if (avg < 600 && avg >= 300) {
        data.color = 1;
    }
    else if (avg >= 0 && avg < 300){
        data.color = 2;
    }
    
    sendMsgToScanQFromISR(data);
    dbgOutputLoc(ADC_ISR_END);
    
    /* Clear ADC Interrupt Flag */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_ADC_1);
}

void IntHandlerDrvTmrInstance0(void)
{
    dbgOutputLoc(TMR_ISR_START);
    count++;
    if (count == 4) 
    {
        struct scanQueueData data;
        data.type = TMR;
        sendMsgToScanQFromISR(data);
        count = 0;
    }
    dbgOutputLoc(TMR_ISR_END);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_3);
}