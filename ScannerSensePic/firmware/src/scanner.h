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

#define MAX_NUMBER_OF_BUFFERS                   10

unsigned int zone;
double red;
unsigned int green;
unsigned int blue;

typedef uintptr_t APP_I2C_DRIVER_HANDLE;
typedef uintptr_t APP_I2C_BUFFER_HANDLE;

typedef enum
{
	SCANNER_STATE_INIT=0,
	SCANNER_STATE_SCANNING,
	SCANNER_READ_WRITE,
} SCANNER_STATES;

typedef struct
{
    SCANNER_STATES state;
    APP_I2C_DRIVER_HANDLE               drvI2CHandle_Master;   
    APP_I2C_BUFFER_HANDLE               drvI2CTxRxBufferHandle[MAX_NUMBER_OF_BUFFERS];
} SCANNER_DATA;

void SCANNER_Initialize ( void );

void SCANNER_Tasks( void );

bool SCANNER_Read_Tasks( void );

#endif /* _SCANNER_H */
