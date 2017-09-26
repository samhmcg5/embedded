#ifndef SCAN_GLOBALS_H
#define	SCAN_GLOBALS_H

// action to be taken by sensing unit
#define STOP    0
#define CONTINUE 1

// data inside Scan's incoming queue
struct scanQueueData
{
    char action;  // indicates next action, stop scanning or continue
    int zone;
};

// incoming nav queue
QueueHandle_t scan_q;

void sendMsgToScanQ(struct scanQueueData msg);

#endif	/* SCAN_GLOBALS_H */

