#ifndef COMMUNICATION_GLOBALS_H
#define	COMMUNICATION_GLOBALS_H

#define UART_RX_QUEUE_SIZE  256
#define UART_TX_QUEUE_SIZE  256

int outgoing_seq = 0;
void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]);

// OUTGOING JSON STRINGS
#define STR_SEQUENCE_ERROR	"{\"SEQ\":%i,\"DELIV_NAV\":{\"STATUS\":1,\"MSG\":\"missing sequence num, expected %u got %u\"}}!"
#define STR_JSON_ERROR		"{\"SEQ\":%i,\"DELIV_NAV\":{\"STATUS\":1,\"MSG\":\"Bad JSON message format\"}}!"
#define STR_UPDATE_POS		"{\"SEQ\":%i, \"DELIV_NAV\": { \"X\":%u, \"Y\":%u } }!"
#define STR_UPDATE_SPEED	"{\"SEQ\":%i,\"DELIV_NAV\":{\"RIGHT_DIR\":%i,\"LEFT_DIR\":%i,\"RIGHT_SPEED\":%u,\"LEFT_SPEED\":%u}}!"
#define STR_IDLE			"{\"SEQ\":%i,\"DELIV_NAV\":{\"STATUS\":0,\"MSG\":\"Rover is IDLE, requesting task\"}}!"

#endif	/* COMMUNICATION_GLOBALS_H */

