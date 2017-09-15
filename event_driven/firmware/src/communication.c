#include "communication.h"
#include "queue.h"

COMMUNICATION_DATA communicationData;

// add a msg to the q

int sendMsgToCommQFromISR(unsigned int msg) 
{
    xQueueSendFromISR(comm_incoming_q, (void*) &msg, NULL);
    return 0;
}

// recv a message from the q

struct queueData recvFromQ() 
{
    BaseType_t success = pdFALSE;
    struct queueData data;
    if (comm_incoming_q != 0) 
    {
        success = xQueueReceive(comm_incoming_q, &data.recv, portMAX_DELAY);
    }
    return data;
}

int sendMsgToUARTQ(unsigned char msg)
{
    xQueueSend(uart_outgoing_q, &msg, portMAX_DELAY);
    return 0;
}

void COMMUNICATION_Initialize(void) 
{
    communicationData.state = COMMUNICATION_STATE_INIT;
    // create the q handle
    comm_incoming_q = xQueueCreate(16, sizeof(unsigned int));
    uart_outgoing_q = xQueueCreate(16, sizeof(unsigned char));

    // start the adc
    DRV_ADC_Open();
    DRV_ADC_Start();
}

void COMMUNICATION_Tasks(void) 
{

    /* Check the application's current state. */
    switch (communicationData.state) 
    {
            /* Application's initial state. */
        case COMMUNICATION_STATE_INIT:
        {
            bool appInitialized = true;
            if (appInitialized) 
            {
                communicationData.state = COMMUNICATION_STATE_SERVICE_TASKS;
            }
            break;
        }

        case COMMUNICATION_STATE_SERVICE_TASKS:
        {
            // read from queue
            struct queueData data;

            // Task Infinite While Loop de loop 
            while (1) 
            {
                data = recvFromQ();
                
                if (data.ret == pdTRUE)
                {
                    // add item to the outgoing UART Queue 
                    sendMsgToUARTQ(data.recv & 0xFF);
                    // set the UART TX interrupt flag
                    SYS_INT_SourceEnable(INT_SOURCE_USART_1_TRANSMIT);
                }
            }

            break;
        }

        default:
        {
            break;
        }
    }
}