#ifndef PERIPHERALS_GLOBALS_H
#define	PERIPHERALS_GLOBALS_H

#define COMMTASK_TO_PERIPHERAL_MSG 0
#define ADC_TO_PERIPHERAL_MSG 1
#define TMR_TO_PERIPHERAL_MSG 2
struct periphQueueData{
    char type;//from adc or commtask
    bool object;//from adc if has object
    int magnet;//from commtask turn on/off magnet
    int distance;
};


void sendMsgToPeriphQ(struct periphQueueData msg);
void sendMsgToPeriphQFromISR(struct periphQueueData msg);

QueueHandle_t periph_q;










#endif	/* PERIPHERALS_GLOBALS_H */


