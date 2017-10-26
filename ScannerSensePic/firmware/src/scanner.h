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

typedef enum
{
	SCANNER_STATE_INIT=0,
	SCANNER_STATE_SCANNING,
} SCANNER_STATES;

typedef struct
{
    SCANNER_STATES state;
} SCANNER_DATA;

unsigned int zone;
double red;
unsigned int green;
unsigned int blue;

void SCANNER_Initialize ( void );

void SCANNER_Tasks( void );

#endif /* _SCANNER_H */
