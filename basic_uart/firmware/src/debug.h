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
#define DBG_LOCATION_1 1
#define DBG_LOCATION_2 2
    
    
int writeToUART(char byte);

int dbgUARTVal(unsigned char outVal);

void dbgOutputLoc(unsigned char outVal);

#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_H */

