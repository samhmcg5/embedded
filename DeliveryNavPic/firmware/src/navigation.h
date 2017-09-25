#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "system_config.h"
#include "system_definitions.h"
#include "queue.h"

typedef enum
{
	NAVIGATION_STATE_INIT=0,
	NAVIGATION_STATE_SERVICE_TASKS,
} NAVIGATION_STATES;

typedef enum
{
    idle=0,
    pickup,
    delivery,
} STATUS;

typedef struct
{
    NAVIGATION_STATES state;
} NAVIGATION_DATA;

void NAVIGATION_Initialize ( void );

void NAVIGATION_Tasks( void );


#endif /* _NAVIGATION_H */
