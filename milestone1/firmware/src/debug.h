/* 
 * File:   debug.h
 * Author: Samuel McGhee
 *
 * Created on September 8, 2017, 3:43 PM
 */

#ifndef DEBUG_H
#define	DEBUG_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#define DBG_LOCATION_COMMTASK_START 1
#define DBG_LOCATION_COMMTASK_WHILE_LOOP 2
#define DBG_LOCATION_COMMTASK_BEFORE_RECV 3
#define DBG_LOCATION_COMMTASK_AFTER_RECV 4
#define DBG_LOCATION_TMRTASK_START 5
#define DBG_LOCATION_TMRTASK_END 6
#define DBG_LOCATION_TMRTASK_BEFORE_SEND 7
#define DBG_LOCATION_TMRTASK_AFTER_SEND 8
    
#define DBG_ERROR_QUEUE_FAILED_TO_CREATE 9
    
void halt(unsigned char outVal);    
    
int writeToUART(char byte);

int dbgUARTVal(unsigned char outVal);

void dbgOutputVal(unsigned char outVal);

void dbgOutputLoc(unsigned char outVal);

void writeUARTString(char * str, int len);

unsigned char charToASCII(unsigned char val);

#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_H */

