#ifndef COMMUNICATION_GLOBALS_H
#define	COMMUNICATION_GLOBALS_H

// UART ISR reads from this Q, we write to it
QueueHandle_t uart_outgoing_q;

void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]);

#endif	/* COMMUNICATION_GLOBALS_H */

