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

unsigned int distR = 0; // measured in ticks
unsigned int distL = 0;
unsigned int total_ticksL = 0;
unsigned int total_ticksR = 0;
unsigned int goalL = 0;
unsigned int goalR = 0;

// incoming motor queue
QueueHandle_t motor_q;
// action queue for right motor
QueueHandle_t right_q;
// action queue for left motor
QueueHandle_t left_q;

typedef enum
{
	MOTOR_CONTROL_STATE_INIT=0,
	MOTOR_CONTROL_HANDLE_INCOMING,
} MOTOR_CONTROL_STATES;

typedef struct
{
    MOTOR_CONTROL_STATES state;
    unsigned char dcL;
    unsigned char dcR;
    unsigned char dirL;
    unsigned char dirR;
} MOTOR_CONTROL_DATA;

void MOTOR_CONTROL_Initialize ( void );

void MOTOR_CONTROL_Tasks( void );

void motorR_recvQInISR(struct pwmQueueData* msg);
void motorL_recvQInISR(struct pwmQueueData* msg);
bool rightQIsEmpty();
bool leftQIsEmpty();

/*
 * fxn: initMotor1and2()
 * -- set GPIO pins to put both motors forward
 * -- enable OC1 and OC2 in PWM mode
 * -- set original duty cycle to 0%
 * -- turn on TIMER2 
*/ 
void initMotors();

// Set motor directions
void setMotorR_Fwd();
void setMotorL_Fwd();
void setMotorR_Bck();
void setMotorL_Bck();

// Set Duty Cycles (number between 0 and 100)
void setMotorR_DC(unsigned char dc);
void setMotorL_DC(unsigned char dc);

void generateActionItems(struct motorQueueData data, struct pwmQueueData * left, struct pwmQueueData * right);
//void handleIncomingMsg(struct motorQueueData data);

#endif /* _MOTOR_CONTROL_H */
