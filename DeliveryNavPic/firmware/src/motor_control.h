#ifndef _MOTOR_CONTROL_H
#define _MOTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

typedef enum
{
	MOTOR_CONTROL_STATE_INIT=0,
	MOTOR_CONTROL_STATE_SERVICE_TASKS,
} MOTOR_CONTROL_STATES;

typedef struct
{
    MOTOR_CONTROL_STATES state;
} MOTOR_CONTROL_DATA;

void MOTOR_CONTROL_Initialize ( void );

void MOTOR_CONTROL_Tasks( void );


#endif /* _MOTOR_CONTROL_H */
