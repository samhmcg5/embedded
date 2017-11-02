#ifndef NAV_GLOBALS_H
#define	NAV_GLOBALS_H

// send to motor
#define ACTION      0
#define TASK        1
// send to UART, coming from motor thread
#define SPEEDS      2
#define POSITION    3
// process data
#define POS_UPDATE  4
#define MAG_REQUEST 5
#define MAG_UPDATE  6

// data inside Nav's incoming queue
struct navQueueData
{
    char type;  // indicates message type (action of task)
    unsigned int a;     // type of action OR color
    unsigned int b;     // action dist OR x destination
    unsigned int c;     // intensity
    unsigned int d;
};

// incoming nav queue
QueueHandle_t nav_q;

void sendMsgToNavQ(struct navQueueData msg);
void sendMsgToNavQFromISR(struct navQueueData msg);

/*
*** initially oriented in x direction = 0 degrees
    |
<--- ---> x
    |
*/
unsigned int tick_ref  = 0;
float posX = 0;
float posY = 20;
int orientation = 0;
unsigned char prev_action = 0xFF;


#endif	/* NAV_GLOBALS_H */

