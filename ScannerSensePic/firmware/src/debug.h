#ifndef DEBUG_H
#define	DEBUG_H

#include "system_config.h"
#include "system_definitions.h"

#define COMM_THREAD_RECVD		0x0
#define SCAN_THREAD_RECVD        0x1

#define COMM_THREAD_WAIT		0x3
#define SCAN_THREAD_WAIT         0x4

#define ISR_UART_RX				0x6
#define ISR_UART_TX				0x7

#define ADC_ISR_START           0x8
#define ADC_ISR_END             0x9

#define TMR_ISR_START           0x10
#define TMR_ISR_END             0x11

void halt(unsigned char outVal);    
    
int writeToUART(unsigned char byte);

int dbgUARTVal(unsigned char outVal);

void dbgOutputVal(unsigned char outVal);

void dbgOutputLoc(unsigned char outVal);

void writeUARTString(char * str, int len);

#endif	/* DEBUG_H */
