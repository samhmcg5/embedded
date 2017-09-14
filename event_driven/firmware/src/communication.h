#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"


typedef enum
{
	COMMUNICATION_STATE_INIT=0,
	COMMUNICATION_STATE_SERVICE_TASKS,
} COMMUNICATION_STATES;


typedef struct
{
    /* The application's current state */
    COMMUNICATION_STATES state;
} COMMUNICATION_DATA;


void COMMUNICATION_Initialize ( void );

void COMMUNICATION_Tasks( void );

// UART message queues
// comms thread reads from this Q
QueueHandle_t comm_incoming_q;
// UART ISR reads from this Q, we write to it
QueueHandle_t uart_outgoing_q;

int sendMsgToCommQFromISR(unsigned int msg);
int sendMsgToUARTQ(unsigned char msg);
unsigned int recvFromQ();

#endif /* _COMMUNICATION_H */