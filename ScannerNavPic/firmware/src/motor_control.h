#ifndef _MOTOR_CONTROL_H
#define _MOTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "queue.h"
#include "motor_globals.h"

#define MOTOR_RIGHT_DIR_PIN    0x4000
#define MOTOR_LEFT_DIR_PIN     0x0002
#define TMR2_PERIOD            3125

// MOTOR 2 === motor on the left when youre facing the USB plugs

// Incoming motor queue
QueueHandle_t motor_q;
// Action queue for right motor
QueueHandle_t right_q;
// Action queue for left motor
QueueHandle_t left_q;

typedef enum
{
	MOTOR_CONTROL_STATE_INIT=0,
	MOTOR_CONTROL_HANDLE_INCOMING,
} MOTOR_CONTROL_STATES;

typedef struct
{
    MOTOR_CONTROL_STATES state;
    unsigned int dcL;
    unsigned int dcR;
    unsigned char speedR;
    unsigned char speedL;
    unsigned char dirL;
    unsigned char dirR;
    unsigned char action;
} MOTOR_CONTROL_DATA;

void MOTOR_CONTROL_Initialize ( void );

void MOTOR_CONTROL_Tasks( void );

void motorR_recvQInISR(struct pwmQueueData* msg);
void motorL_recvQInISR(struct pwmQueueData* msg);
bool rightQIsEmpty();
bool leftQIsEmpty();

void initMotors();

// Set motor directions
void setMotorR_Fwd();
void setMotorL_Fwd();
void setMotorR_Bck();
void setMotorL_Bck();

// Set Duty Cycles (number between 0 and 100)
void setMotorR_DC(unsigned int dc);
void setMotorL_DC(unsigned int dc);

void generateActionItems(struct motorQueueData data, struct pwmQueueData * left, struct pwmQueueData * right);

void readFromQandSetPins(unsigned char motor);

void updateLocation(unsigned int cm, unsigned char action);
void handleIncomingMsg(struct motorQueueData data);

#endif /* _MOTOR_CONTROL_H */