#ifndef COMMUNICATION_GLOBALS_H
#define	COMMUNICATION_GLOBALS_H

// UART Queue sizes
#define UART_RX_QUEUE_SIZE  256
#define UART_TX_QUEUE_SIZE  256

// OUTGOING JSON STRINGS
// Error strings
#define STR_SEQUENCE_ERROR	"{\"SEQ\":%i,\"SCAN_NAV\":{\"STATUS\":1,\"MSG\":\"Missing sequence number, expected %u got %u\"}}!"
#define STR_JSON_ERROR		"{\"SEQ\":%i,\"SCAN_NAV\":{\"STATUS\":1,\"MSG\":\"Bad JSON message format\"}}!"
// Status strings
#define STR_UPDATE_POS		"{\"SEQ\":%i, \"SCAN_NAV\": { \"DIST\": %u, \"STATE\": \"%s\" }}!"
#define STR_IDLE			"{\"SEQ\":%i,\"SCAN_NAV\":{\"STATUS\":0,\"MSG\":\"Rover is IDLE, requesting task\"}}!" // NOT USED

// Recieved motor control - DEBUG - NOT USED
#define STR_COMM_ACTION     "{\"SERVER UPDATE\": \"SEQ\":%i, \"ACT\":%i, \"DIST\":%i, \"SPEED\":%i }!"
#define STR_SENSOR_ACTION   "{\"LINE SENSOR UPDATE\": \"SEQ\":%i, \"ACT\":%i, \"DIST\":%i, \"SPEED\":%i }!"

// Sequence number counter
int outgoing_seq = 0;
// Enables line sensor - DEBUG - NOT USED
int sensor_enable = 0;
// Global UART communication function
void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]);

#endif	/* COMMUNICATION_GLOBALS_H */

