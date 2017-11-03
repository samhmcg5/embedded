#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "system_config.h"
#include "system_definitions.h"
#include "nav_globals.h"
#include "queue.h"

/* LOCATION DEFINITIONS */
#define RED_X   20
#define GREEN_X 40
#define BLUE_X  60
#define PROD_Y  8

#define ALPHA_X 10
#define BETA_X  30
#define GAMMA_X 50
#define DELIV_Y 42

#define CRASH_MARGIN_L 15
#define CRASH_MARGIN_H 35

/* MAGNET STATES */
#define OFF 0
#define ON  1

#define PI 3.14159265
const double convert = PI / 180.0;

// incoming nav queue
QueueHandle_t nav_q;

typedef enum
{
	NAVIGATION_STATE_INIT=0,
	NAVIGATION_STATE_SERVICE_TASKS,
} NAVIGATION_STATES;

typedef enum
{
    idle=0,
    pickup,
    magnet_on,
    delivery,
    magnet_off,
    reverse,
} STATUS;

typedef struct
{
    NAVIGATION_STATES state;
    STATUS status;
    char magnet_should_be;
    char magnet_is;
    bool stopped;
    bool ir_used;
} NAVIGATION_DATA;

void NAVIGATION_Initialize ( void );

void NAVIGATION_Tasks( void );

void handleIncomingMsg(struct navQueueData data);
void handleTaskState();

void updateLocation(unsigned int cm, unsigned char action);

void beginTask(struct navQueueData task);

unsigned int getNearestVertical();
void getOutOfCrashZone();
void setOrientation(unsigned int theta, unsigned char speed);
int getPickupDX(unsigned int zone);

#endif /* _NAVIGATION_H */
