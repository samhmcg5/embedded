#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "queue.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
    
    // For the message Queue
    QueueHandle_t q;
    int sendMsgToQ(unsigned char msg);
    
typedef enum
{
	COMMUNICATION_STATE_INIT=0,
	COMMUNICATION_STATE_RUNNING
} COMMUNICATION_STATES;

typedef struct
{
    COMMUNICATION_STATES state;
} COMMUNICATION_DATA;



void COMMUNICATION_Initialize ( void );

void COMMUNICATION_Tasks( void );


#endif /* _COMMUNICATION_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif