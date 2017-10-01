#ifndef DEBUG_H
#define	DEBUG_H

#include "system_config.h"
#include "system_definitions.h"

#define COMM_THREAD_RECVD		0x0
#define COMM_THREAD_WAIT		0x1
#define ISR_UART_RX				0x2
#define ISR_UART_TX				0x3
#define PERIPH_MSGQ_FAIL        0x4
#define ISR_ADC_BEGIN           0x5
#define ISR_ADC_END             0x6
#define ISR_TMR_START           0x7
#define ISR_TMR_END             0x8
#define PERIPH_START            0x9
#define PERIPH_COMM             0xA
#define PERIPH_ADC              0xB
#define PERIPH_TMR              0xC
#define ISR_UART_TX_WHILE       0xD
#define ISR_UART_TX_BUFF_FULL   0xE



void halt(unsigned char outVal);    
    
int writeToUART(unsigned char byte);

int dbgUARTVal(unsigned char outVal);

void dbgOutputVal(unsigned char outVal);

void dbgOutputLoc(unsigned char outVal);

void writeUARTString(char * str, int len);

#endif	/* DEBUG_H */
