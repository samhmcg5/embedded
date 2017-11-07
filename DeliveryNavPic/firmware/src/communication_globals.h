#ifndef COMMUNICATION_GLOBALS_H
#define COMMUNICATION_GLOBALS_H

int outgoing_seq = 0;
void commSendMsgToUartQueue(unsigned char msg[UART_TX_QUEUE_SIZE]);

// OUTGOING JSON STRINGS
#define STR_SEQUENCE_ERROR  "{\"SEQ\":%i,\"DELIV_NAV\":{\"ERROR\":1,\"MSG\":\"Missing sequence num, expected %u got %u\"}}!"
#define STR_JSON_ERROR      "{\"SEQ\":%i,\"DELIV_NAV\":{\"ERROR\":2,\"MSG\":\"Bad JSON message format\"}}!"
#define STR_UPDATE_POS      "{\"SEQ\":%i, \"DELIV_NAV\": { \"X\":%u, \"Y\":%u, \"OR\":%u } }!"
#define STR_UPDATE_SPEED    "{\"SEQ\":%i,\"DELIV_NAV\":{\"RIGHT_DIR\":%i,\"LEFT_DIR\":%i,\"RIGHT_SPEED\":%u,\"LEFT_SPEED\":%u}}!"
#define STR_IDLE            "{\"SEQ\":%i,\"DELIV_NAV\":{\"STATUS\":0,\"MSG\":\"Rover is IDLE, requesting task\"}}!"
#define STR_EXECUTING       "{\"SEQ\":%i,\"DELIV_NAV\":{\"STATUS\":0,\"MSG\":\"Task EXECUTING\"}}!"
#define STR_ACK_TASK        "{\"SEQ\":%i,\"DELIV_NAV\":{\"TASK\":\"RECIEVED\"}}!"
#define STR_ACK_ACT         "{\"SEQ\":%i,\"DELIV_NAV\":{\"ACTION\":\"RECIEVED\"}}!"
#define STR_DATA_REQ        "{\"SEQ\":%i,\"DELIV_NAV\":{\"SET_MAGNET\":%u, \"IR_DATA\":%u}}!"

#define STR_DBG             "{\"SEQ\":%i,\"DELIV_NAV\":{\"STATE\":%u, \"IR\":%u, \"MAG\":%u}}!"

#endif  /* COMMUNICATION_GLOBALS_H */

