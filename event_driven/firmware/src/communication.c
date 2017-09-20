#include "communication.h"
#include "queue.h"

COMMUNICATION_DATA communicationData;

// recv a message from the q
struct queueData recvFromQ() 
{
    BaseType_t success = pdFALSE;
    struct queueData data;
    if (comm_incoming_q != 0) 
    {
        success = xQueueReceive(comm_incoming_q, &data.recv, portMAX_DELAY);
        data.ret = success;
    }
    return data;
}
//send into commtask from isr
void commSendMsgFromISR(unsigned char msg[UART_RX_QUEUE_SIZE]) 
{
    //send to comms task from isr
    xQueueSendToBackFromISR(comm_incoming_q, msg, NULL);
}

//send into commtask
void commSendMsg(unsigned char msg[UART_RX_QUEUE_SIZE]) 
{
    //send to the comms task
    xQueueSendToBack(comm_incoming_q, msg, portMAX_DELAY);
}
//send each byte of message into uart out queue
void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]) 
{
    int i;
    for(i = 0; i< strlen(msg); i++)
    {
        //each byte of message is sent into uartqueue
        xQueueSendToBack(uart_outgoing_q, &msg[i], portMAX_DELAY);
    }
    Nop();
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
}

void uartReceiveFromOutQueueInISR(unsigned char* msg) 
{
    //read from uart queue to transmit
    xQueueReceiveFromISR(uart_outgoing_q, msg, NULL);
}

bool checkIfSendQueueIsEmpty() 
{
    return xQueueIsQueueEmptyFromISR(uart_outgoing_q);
}

unsigned char commBuffer[UART_RX_QUEUE_SIZE];
unsigned int commBufferIdx = 0;

void readUartReceived() 
{
    Nop();
    unsigned char recv = PLIB_USART_ReceiverByteReceive(USART_ID_1);

    Nop();
    if (recv == '}') 
    {
        //change this to our delimiter
        if(commBufferIdx > 0){
            if(commBuffer[commBufferIdx - 1] == '{')
            {
                commBuffer[0] = '\0';
                commBufferIdx = 0;
            }
            else
            {
                commBuffer[commBufferIdx] = recv;
                commSendMsgFromISR(commBuffer); ////FOR FUCKS SACKE UNCOMMENT
                commBuffer[0] = '\0';
            }
        }
    } 
    else if (recv == '{') 
    {
        //        bufferToWrite2 = "";
        commBufferIdx = 0;
        commBuffer[commBufferIdx] = recv;
        commBufferIdx++;
    } 
    else 
    {
        commBuffer[commBufferIdx] = recv;
        commBufferIdx++;
        if(commBuffer[0] != '{')
        {
            commBuffer[0] = '\0';
            commBufferIdx = 0;
        }
    }
}

void uartWriteMsg(char writeBuff) {
    PLIB_USART_TransmitterByteSend(USART_ID_1, writeBuff); //write a byte
}

void COMMUNICATION_Initialize(void) 
{
    communicationData.state = COMMUNICATION_STATE_INIT;
    // create the q handle
     
    comm_incoming_q = xQueueCreate(16, sizeof (unsigned char[UART_RX_QUEUE_SIZE]));//general purpose for all incoming
    uart_outgoing_q = xQueueCreate(UART_TX_QUEUE_SIZE, sizeof (unsigned char));

    DRV_TMR0_Start();
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
            unsigned char rec[UART_RX_QUEUE_SIZE];
            while (1) 
            {
                if(xQueueReceive(comm_incoming_q, &rec, portMAX_DELAY))
                {
                    commSendMsgToUartQueue("Team 14 \n\r");                
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