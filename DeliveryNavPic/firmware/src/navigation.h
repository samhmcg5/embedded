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
#define RED_X   30
#define GREEN_X 60
#define BLUE_X  95
const unsigned int PICKUP_ZONES[] = {RED_X, GREEN_X, BLUE_X};
#define PROD_Y  40

#define ALPHA_X 36
#define BETA_X  76
#define GAMMA_X 110
const unsigned int DROP_ZONES[] = {ALPHA_X, BETA_X, GAMMA_X};
#define DELIV_Y 25

// #define CRASH_MARGIN_L 15
// #define CRASH_MARGIN_H 27

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
    idle      =0,
    pickup    =1,
    magnet_on =2,
    reverse1  =3,
    delivery  =4,
    magnet_off=5,
    reverse2  =6,
    sweep     =7,
} STATUS;

typedef struct
{
    NAVIGATION_STATES state;
    STATUS status;
    char magnet_should_be;
    char magnet_is;
    unsigned int ir_dist;
    bool stopped;
    bool ir_used;
    unsigned int from;
    unsigned int to;
} NAVIGATION_DATA;

bool generated = false;
bool good_to_go = false;
unsigned int ir_trav = 0;
unsigned int beg_orient = 0;

void NAVIGATION_Initialize ( void );

void NAVIGATION_Tasks( void );

void handleIncomingMsg(struct navQueueData data);
void handleTaskState();

void updateLocation(unsigned int cm, unsigned char action);

void beginTask(unsigned int task);

unsigned int getNearestVertical(unsigned int o);
// void getOutOfCrashZone(int x, int y, int o, int *future_x, int *future_y, int *future_o);
void setOrientation(unsigned int goal, unsigned int current ,unsigned char speed);
void generateDeliveryDirs(unsigned int zone);

#endif /* _NAVIGATION_H */
