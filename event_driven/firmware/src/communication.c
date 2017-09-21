#include <stdio.h>
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

// JSMN
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) 
{
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) 
    {
		return 0;
	}
	return -1;
}

int parseJSON (unsigned char rec[UART_RX_QUEUE_SIZE]) 
{
    JSON_STRING = rec;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */
    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
                    
    // PARSING TOKENS AND SENDING BACK FOR DEBUG 
    jsmntok_t key;
    unsigned int length;
    char keyString[length + 1];
    
    int i;
                    
    key = t[2];
    length = key.end - key.start;
    keyString[length + 1];    
    memcpy(keyString, &JSON_STRING[key.start], length);
    keyString[length] = '\0';
    
    return atoi(keyString);
    
    
//    int j;
//    for (j = 0; j < r; ++j)
//    {
//        key = t[j];
//        length = key.end - key.start;
//        keyString[length + 1];    
//        memcpy(keyString, &JSON_STRING[key.start], length);
//        keyString[length] = '\0';
//        commSendMsgToUartQueue(keyString);
//    }    
}

char commBuffer[UART_RX_QUEUE_SIZE];
unsigned int commBufferIdx = 0;

void readUartReceived() 
{
    unsigned char recv = PLIB_USART_ReceiverByteReceive(USART_ID_1);
    // get the start delimeter
    if (recv == '{')
    {
        commBufferIdx = 0;
        commBuffer[commBufferIdx] = recv;
        commBufferIdx++;
    }
    // end token, add to the queue and exit
    else if (recv == '}')
    {
        commBuffer[commBufferIdx] = recv;
        commBufferIdx++;
        commBuffer[commBufferIdx] = '\0';
        commSendMsgFromISR(commBuffer);
    }
    else
    {
        commBuffer[commBufferIdx] = recv;
        commBufferIdx++;
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
                    int direction = parseJSON(rec);
                    direction++;
                    char buf[32];
                    sprintf(buf, "{\"ACK\" : %i}", direction);
                    commSendMsgToUartQueue(buf);
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