#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "system_config.h"
#include "system_definitions.h"
#include "nav_globals.h"
#include "queue.h"

typedef enum
{
	NAVIGATION_STATE_INIT=0,
	NAVIGATION_STATE_SERVICE_TASKS,
} NAVIGATION_STATES;

typedef enum
{
    idle=0,
    moving,
} STATUS;

typedef struct
{
    NAVIGATION_STATES state;
    STATUS status;
} NAVIGATION_DATA;

void NAVIGATION_Initialize ( void );

void NAVIGATION_Tasks( void );

void handleIncomingMsg(struct navQueueData data);


#endif /* _NAVIGATION_H */
