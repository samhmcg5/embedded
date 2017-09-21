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

#define UART_RX_QUEUE_SIZE 256
#define UART_TX_QUEUE_SIZE 256
#define SERVER_TIMOUT       4 // number of quarter seconds

typedef enum
{
	COMMUNICATION_STATE_INIT=0,
	COMMUNICATION_STATE_SERVICE_TASKS,
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

struct queueData
{
   unsigned char recv[UART_RX_QUEUE_SIZE];
    BaseType_t ret;
};

void COMMUNICATION_Initialize ( void );

void COMMUNICATION_Tasks( void );

// UART message queues
// comms thread reads from this Q
QueueHandle_t comm_incoming_q;
// UART ISR reads from this Q, we write to it
QueueHandle_t uart_outgoing_q;
// server reply timout interrupt queue
QueueHandle_t timeout_q;

//RX functions
struct queueData recvFromQ();
int sendMsgToCommQFromISR(unsigned int msg);
void commSendMsgFromISR(unsigned char msg[UART_RX_QUEUE_SIZE]);
void commSendMsg(unsigned char msg[UART_RX_QUEUE_SIZE]);
void readUartReceived();

//TX functions
void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]);
bool checkIfSendQueueIsEmpty();
void uartReceiveFromOutQueueInISR(unsigned char *msg);
void uartWriteMsg(char writeBuff);

//JSMN functions
static const char *JSON_STRING;
static int jsoneq(const char *json, jsmntok_t *tok, const char *s);


#endif /* _COMMUNICATION_H */