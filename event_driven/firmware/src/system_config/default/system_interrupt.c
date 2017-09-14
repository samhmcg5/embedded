#include "system/common/sys_common.h"
#include "communication.h"
#include "system_definitions.h"

#define ADC_NUM_SAMPLES     16

// pull the front item off the outgoing uart queue
unsigned char recvFromUARTQ() 
{
    BaseType_t success = pdFALSE;
    unsigned int recv;
    if (comm_incoming_q != 0) 
    {
        success = xQueueReceiveFromISR(uart_outgoing_q, (void *)&recv, NULL);
    }
    return recv;
}

void IntHandlerDrvUsartInstance0(void)
{
    // receive available data from UART
    while (!DRV_USART0_ReceiverBufferIsEmpty())
    {
        unsigned char byte = DRV_USART0_ReadByte(); 
        sendMsgToCommQFromISR(byte);       
    }
    // send messages from outgoing queue to UART interface 
    unsigned char recv;
    while (pdFALSE == xQueueIsQueueEmptyFromISR(uart_outgoing_q))
    {
        recv = recvFromUARTQ();
        DRV_USART0_WriteByte(recv);
    }
    
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_TRANSMIT);
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_RECEIVE);
    SYS_INT_SourceStatusClear(INT_SOURCE_USART_1_ERROR);
    
    //DRV_USART_TasksTransmit(sysObj.drvUsart0);
    //DRV_USART_TasksError(sysObj.drvUsart0);
    //DRV_USART_TasksReceive(sysObj.drvUsart0);
}
 
void IntHandlerDrvAdc(void)
{
    // take the average over the adc registers
    unsigned int avg = 0;
    int i;
    for (i=0; i<ADC_NUM_SAMPLES; i++)
    {
        avg += PLIB_ADC_ResultGetByIndex(ADC_ID_1, i);
    }
    avg = avg / ADC_NUM_SAMPLES;
    
    // now pop it on the uart outgoing queue
    sendMsgToCommQFromISR(avg);
    
    /* Clear ADC Interrupt Flag */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_ADC_1);
}
