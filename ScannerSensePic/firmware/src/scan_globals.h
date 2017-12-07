#ifndef SCAN_GLOBALS_H
#define	SCAN_GLOBALS_H

// action to be taken by sensing unit
#define COMM    0
#define ADC     1
#define TMR     2
#define UART    3

// distance threshold of colored blocks
#define OBJ_DISTANCE 10.0
#define OBJ_WIDTH    12

// data inside scanners incoming queue
struct scanQueueData
{
    char type;
    unsigned int action;
    unsigned int zone;
    unsigned int xPos;
    double dist;
    unsigned int sync;
    unsigned int checksum;
    unsigned int signature;
    unsigned int objX;
    unsigned int objY;
    unsigned int objWidth;
    unsigned int objHeight;
};

// incoming scan queue
QueueHandle_t scan_q;

void sendMsgToScanQ(struct scanQueueData msg);

void sendMsgToScanQFromISR(struct scanQueueData msg);

// for use in ADC ISR
double voltsToCm(double volts);

#endif	/* SCAN_GLOBALS_H */

