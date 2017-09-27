#ifndef _LINE_SENSOR_H
#define _LINE_SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "queue.h"

// incoming motor queue
QueueHandle_t sensor_q;

struct sensorQueueData
{
    char front_sensor;
    char rear_sensor;
};

void sendMsgToSensorQFromISR(struct sensorQueueData msg);
void sensorHandleIncomingMsg(struct sensorQueueData data);

typedef enum
{
	/* Application's state machine's initial state. */
	LINE_SENSOR_STATE_INIT=0,
	LINE_SENSOR_STATE_SERVICE_TASKS,

	/* TODO: Define states used by the application state machine. */

} LINE_SENSOR_STATES;

typedef struct
{
    /* The application's current state */
    LINE_SENSOR_STATES state;

    /* TODO: Define any additional data used by the application. */

} LINE_SENSOR_DATA;

void LINE_SENSOR_Initialize ( void );

void LINE_SENSOR_Tasks( void );

#endif
