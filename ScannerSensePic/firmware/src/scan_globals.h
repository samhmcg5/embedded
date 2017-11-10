#ifndef SCAN_GLOBALS_H
#define	SCAN_GLOBALS_H

// action to be taken by sensing unit
#define INFO    0
#define ADC     1
#define I2C     2
#define TMR     3

// data inside scanners incoming queue
struct scanQueueData
{
    char type;
    unsigned int action;
    unsigned int zone;
    unsigned int dist;
    unsigned int x;
};

// incoming scan queue
QueueHandle_t scan_q;

void sendMsgToScanQ(struct scanQueueData msg);

void sendMsgToScanQFromISR(struct scanQueueData msg);

double voltsToCm(double volts);

#endif	/* SCAN_GLOBALS_H */

