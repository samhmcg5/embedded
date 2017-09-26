#ifndef NAV_GLOBALS_H
#define	NAV_GLOBALS_H

// send to motor
#define ACTION      0
#define TASK        1
// send to UART, coming from motor thread
#define SPEEDS      2
#define POSITION    3

// data inside Nav's incoming queue
struct navQueueData
{
    char type;  // indicates message type (action of task)
    char a;     // type of action OR color
    char b;     // action dist OR x destination
    unsigned int c;     // intensity
    unsigned int d;
};

// incoming nav queue
QueueHandle_t nav_q;

void sendMsgToNavQ(struct navQueueData msg);
void sendMsgToNavQFromISR(struct navQueueData msg);

#endif	/* NAV_GLOBALS_H */

