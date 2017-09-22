#ifndef NAV_GLOBALS_H
#define	NAV_GLOBALS_H

// data inside Nav's incoming queue
struct navQueueData
{
    char msg;   // indicates message type
    char x;     // usually destination x    
    char y;     // est y
    char color; // color of block to move??? maybe redundant
};

// incoming nav queue
QueueHandle_t nav_q;

void sendMsgToNavQ(struct navQueueData msg);

#endif	/* NAV_GLOBALS_H */

