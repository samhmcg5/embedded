#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "system_config.h"
#include "system_definitions.h"
#include "scan_globals.h"
#include "queue.h"
#include "i2c.h"

unsigned int prevzone;
unsigned int zone;
unsigned int red;
unsigned int green;
unsigned int blue;
unsigned int prevEdge;
unsigned int currXPos;

typedef enum
{
	SCANNER_STATE_INIT=0,
	SCANNER_STATE_SCANNING,
	SCANNER_READ_WRITE,
} SCANNER_STATES;

typedef struct
{
    SCANNER_STATES state;
    DRV_HANDLE i2c_handle;
} SCANNER_DATA;

void SCANNER_Initialize ( void );

void SCANNER_Tasks( void );

#endif /* _SCANNER_H */
