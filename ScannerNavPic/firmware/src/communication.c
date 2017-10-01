#include <stdio.h>
#include "communication.h"
#include "communication_globals.h"
#include "queue.h"
#include "motor_control.h"
#include "motor_globals.h"

// communication's state
COMMUNICATION_DATA communicationData;

//send into commtask from isr
void commSendMsgFromISR(unsigned char msg[UART_RX_QUEUE_SIZE]) 
{
    //send to comms task from isr
    xQueueSendToBackFromISR(comm_incoming_q, msg, NULL);
}

//send each byte of message into uart out queue
void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]) 
{
    int i;
    for(i = 0; i< strlen(msg); i++)
    {
        //each byte of message is sent into uart queue
        xQueueSendToBack(uart_outgoing_q, &msg[i], portMAX_DELAY);
    }
    outgoing_seq++;
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

int getIntFromKey(jsmntok_t key)
{
    unsigned int length = key.end - key.start;
    char keyString[length + 1];    
    memcpy(keyString, &JSON_STRING[key.start], length);
    keyString[length] = '\0';
    return atoi(keyString);
}

struct motorQueueData parseJSON (unsigned char rec[UART_RX_QUEUE_SIZE]) 
{
    struct motorQueueData out;
    
    JSON_STRING = rec;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */
    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
    
    /* Get the sequence ID number */
    int seq = getIntFromKey(t[2]);
    /* Is it the expected next sequence? */
    if (seq != prev_inc_seq + 1) // ERROR
    {
        char buf[128];
        sprintf(buf, STR_SEQUENCE_ERROR, outgoing_seq, prev_inc_seq+1, seq);
        commSendMsgToUartQueue(buf);
    }
    prev_inc_seq = seq;

    if(r == 9) // ACTION
    {
        out.action = getIntFromKey(t[4]);
        out.dist = getIntFromKey(t[6]);
        out.speed = getIntFromKey(t[8]);
    }
  
    else // ERROR
    {
        // send message back to server something is wrong
        char buf[128];
        sprintf(buf, STR_JSON_ERROR, outgoing_seq);
        commSendMsgToUartQueue(buf);
    }
    return out;
}

// called from the ISR
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

void uartWriteMsg(char writeBuff) 
{
    PLIB_USART_TransmitterByteSend(USART_ID_1, writeBuff); //write a byte
}

void COMMUNICATION_Initialize(void) 
{
    communicationData.state = COMMUNICATION_STATE_INIT;
    communicationData.data = 5000;
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
                communicationData.state = COMM_AWAIT_RESPONSE;
            }
            break;
        }
        case COMM_AWAIT_RESPONSE:
        {
            dbgOutputLoc(COMM_THREAD_WAIT);
            
            unsigned char rec[UART_RX_QUEUE_SIZE];
            if(xQueueReceive(comm_incoming_q, &rec, portMAX_DELAY))
            {
                dbgOutputLoc(COMM_THREAD_RECVD);
                
                struct  motorQueueData out = parseJSON(rec);
                sendMsgToMotorQ(out);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}