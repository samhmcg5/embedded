#ifndef NAV_GLOBALS_H
#define	NAV_GLOBALS_H

// Data types for for the Nav Queue
#define ACTION      0
#define TASK        1
#define SPEEDS      2
#define POSITION    3
#define POS_UPDATE  4
#define DATA_REQ    5
#define DATA_UPDATE  6

// data inside Nav's incoming queue
struct navQueueData
{
    char type;          // indicates message type (action of task)
    unsigned int a;     // type of action OR color
    unsigned int b;     // action dist OR x destination
    unsigned int c;     // intensity
    unsigned int d;
};

void sendMsgToNavQ(struct navQueueData msg);
void sendMsgToNavQFromISR(struct navQueueData msg);

float posX = 0;
float posY = 20;
int orientation = 0;
unsigned char prev_action = 0xFF;
bool task_done = false;

#endif	/* NAV_GLOBALS_H */

