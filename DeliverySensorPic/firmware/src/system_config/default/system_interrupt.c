#include "system/common/sys_common.h"
#include "communication.h"
#include "system_definitions.h"
#include "peripherals_globals.h"


#define ADC_NUM_SAMPLE_PER_AVERAGE     16
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
            dbgOutputLoc(ISR_UART_TX_WHILE);
            if (!checkIfSendQueueIsEmpty()) 
            {
                unsigned char writeBuff;
                if (PLIB_USART_TransmitterBufferIsFull(USART_ID_1)) 
                {
                    dbgOutputLoc(ISR_UART_TX_BUFF_FULL);
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
    dbgOutputLoc(ISR_ADC_BEGIN);
    
    
      ADC_SAMPLE avg = 0;
    // avg = 0;
    uint8_t i = 0;

    for (i = 0; i < ADC_NUM_SAMPLE_PER_AVERAGE; i++) {
        avg += PLIB_ADC_ResultGetByIndex(ADC_ID_1, i);
    }
    avg = avg / 16;
    
    
    /* Clear ADC Interrupt Flag */
    Nop();
    struct periphQueueData msg;
    msg.type = ADC_TO_PERIPHERAL_MSG;
    msg.object = true;//has object
    msg.distance = avg; //within threshold
    sendMsgToPeriphQFromISR(msg);
    Nop();
    
    dbgOutputLoc(ISR_ADC_END);
    
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_ADC_1);
     PLIB_ADC_SampleAutoStartEnable(ADC_ID_1);
}
    
    
    
    
    
void IntHandlerDrvTmrInstance0(void)
{
    dbgOutputLoc(ISR_TMR_START);
    Nop();
    struct periphQueueData msg;
    msg.type = TMR_TO_PERIPHERAL_MSG;
    sendMsgToPeriphQFromISR(msg);
    Nop();
    dbgOutputLoc(ISR_TMR_END);
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
