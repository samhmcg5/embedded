#ifndef DEBUG_H
#define	DEBUG_H

#include "system_config.h"
#include "system_definitions.h"


#define COMM_THREAD_WAIT		0x0
#define COMM_THREAD_RECVD		0x1
#define COMM_THREAD_PARSED      0x2

#define MOTOR_THREAD_RECVD      0x3
#define MOTOR_THREAD_WAIT       0x5

#define ISR_UART_RX				0x6
#define ISR_UART_TX				0x7

#define ISR_MOTOR_L_START		0x8
#define ISR_MOTOR_R_START		0x9
#define ISR_MOTOR_L_DONE		0xA
#define ISR_MOTOR_R_DONE		0xB
#define ISR_MOTOR_L_Q_READ		0xC
#define ISR_MOTOR_R_Q_READ		0xD

#define MOTOR_THR_SEND_TO_Q_L	0xE
#define MOTOR_THR_SEND_TO_Q_R	0xF

#define PARSE_JSON_START        0x10
#define PARSE_JSON_END          0x12
#define PARSE_JSON_ACTION       0x11


void halt(unsigned char outVal);    
    
int writeToUART(unsigned char byte);

int dbgUARTVal(unsigned char outVal);

void dbgOutputVal(unsigned char outVal);

void dbgOutputLoc(unsigned char outVal);

void writeUARTString(char * str, int len);

#endif	/* DEBUG_H */
