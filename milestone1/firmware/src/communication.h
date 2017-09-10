#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "queue.h"
#include "debug.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif

// state machine call inside timer interrupt 
typedef enum 
{
    TMR_MESSAGE_STATE_T=0,
    TMR_MESSAGE_STATE_E,
    TMR_MESSAGE_STATE_A,
    TMR_MESSAGE_STATE_M,
    TMR_MESSAGE_STATE_1,
    TMR_MESSAGE_STATE_4,
    TMR_MESSAGE_STATE_DEFAULT
} TMR_MESSAGE_STATES;
    
typedef struct 
{
    TMR_MESSAGE_STATES msg_state;
}MESSAGE_DATA;

MESSAGE_DATA tmr_message;

typedef enum
{
	COMMUNICATION_STATE_INIT=0,
	COMMUNICATION_STATE_RUNNING
} COMMUNICATION_STATES;

typedef struct
{
    COMMUNICATION_STATES state;
} COMMUNICATION_DATA;


// For the message Queue
QueueHandle_t q;
int sendMsgToQ(unsigned char msg);
unsigned char recvFromQ();

void COMMUNICATION_Initialize ( void );

void COMMUNICATION_Tasks( void );


#endif /* _COMMUNICATION_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif