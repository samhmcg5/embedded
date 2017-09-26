#ifndef COMMUNICATION_GLOBALS_H
#define	COMMUNICATION_GLOBALS_H

int outgoing_seq = 0;
void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]);

// OUTGOING JSON STRINGS
#define STR_SEQUENCE_ERROR	"{\"SEQ\":%i,\"SCAN_SENSE\":{\"STATUS\":1,\"MSG\":\"missing sequence num, expected %u got %u\"}}!"
#define STR_JSON_ERROR		"{\"SEQ\":%i,\"SCAN_SENSE\":{\"STATUS\":1,\"MSG\":\"Bad JSON message format\"}}!"
#define STR_UPDATE_ZONE_ACTION	"{\"SEQ\":%i,\"SCAN_SENSE\":{\"ZONE\":%i,\"ACTION\":%i}}!"
#define STR_IDLE			"{\"SEQ\":%i,\"SCAN_SENSE\":{\"STATUS\":0,\"MSG\":\"Rover is IDLE, requesting task\"}}!"

#endif	/* COMMUNICATION_GLOBALS_H */

