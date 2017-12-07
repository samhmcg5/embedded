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
#include "pixy.h"

typedef enum
{
	SCANNER_STATE_INIT=0,
	SCANNER_STATE_SCANNING,
} SCANNER_STATES;

typedef struct
{
    SCANNER_STATES state;
    // flag for IR sensor detecting first object
    bool detectionFlag; //  
    // previous and current zone information
    unsigned int prevZone;
    unsigned int currZone;
    // current zone object color information
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    // last detected object information and rover position information
    unsigned int prevEdge;
    unsigned int currXPos;
    // last detected object width
    unsigned int width;
    // last detected object color
    unsigned int signature;
    // color has been detected for object
    bool colorFlag;
    bool objectDetected;
    
} SCANNER_DATA;

SCANNER_DATA scannerData;

int abs(int x);

void sendMsgToComm(unsigned int zone, unsigned int red, unsigned int green, unsigned int blue, char *msg);

void SCANNER_Initialize ( void );

void SCANNER_Tasks( void );

#endif /* _SCANNER_H */
