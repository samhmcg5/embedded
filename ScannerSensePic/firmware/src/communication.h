#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "system/system.h"
#include "system/clk/sys_clk.h"
#include "driver/usart/drv_usart.h"
#include "system/devcon/sys_devcon.h"
#include <sys/appio.h>
#include <GenericTypeDefs.h>
#include "system/common/sys_module.h"
#include "system/msg/sys_msg.h"
#include "debug.h"
#include "jsmn.h"
#include "string.h"

#define UART_RX_QUEUE_SIZE  256
#define UART_TX_QUEUE_SIZE  256

typedef enum
{
	COMMUNICATION_STATE_INIT=0,
    COMM_REQUEST_SERVER,
    COMM_AWAIT_RESPONSE,
    COMM_ACK_SERVER
} COMMUNICATION_STATES;


typedef struct
{
    /* The application's current state */
    COMMUNICATION_STATES state;
    int recvd;
    int data;
} COMMUNICATION_DATA;

void COMMUNICATION_Initialize ( void );

void COMMUNICATION_Tasks( void );

// UART message queues
// comms thread reads from this Q
QueueHandle_t comm_incoming_q;
// UART ISR reads from this Q, we write to it
QueueHandle_t uart_outgoing_q;

//RX functions
int sendMsgToCommQFromISR(unsigned int msg);
void commSendMsgFromISR(unsigned char msg[UART_RX_QUEUE_SIZE]);
void readUartReceived();

//TX functions
bool checkIfSendQueueIsEmpty();
void uartReceiveFromOutQueueInISR(unsigned char *msg);
void uartWriteMsg(char writeBuff);

// Used to parse incoming
char commBuffer[UART_RX_QUEUE_SIZE];
unsigned int commBufferIdx = 0;

// used to check messages
int prev_inc_seq = 0;

//JSMN functions
static const char *JSON_STRING;
static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
struct navQueueData parseJSON (unsigned char rec[UART_RX_QUEUE_SIZE]);
int getIntFromKey(jsmntok_t key); 


#endif /* _COMMUNICATION_H */