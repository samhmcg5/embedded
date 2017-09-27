#ifndef SCAN_GLOBALS_H
#define	SCAN_GLOBALS_H

// action to be taken by sensing unit
#define INFO    0
#define ADC     1
#define TMR     2

// data inside scanners incoming queue
struct scanQueueData
{
    char type;
    unsigned int action;
    unsigned int zone;
    unsigned int color;
};

// incoming scan queue
QueueHandle_t scan_q;

void sendMsgToScanQ(struct scanQueueData msg);

void sendMsgToScanQFromISR(struct scanQueueData msg);

#endif	/* SCAN_GLOBALS_H */

