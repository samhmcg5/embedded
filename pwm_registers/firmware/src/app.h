#ifndef _APP_H
#define _APP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#define MOTOR1_DIR_PIN     0x4000
#define MOTOR2_DIR_PIN     0x0002
#define TMR2_PERIOD        3125

// MOTOR 2 === motor on the left when youre facing the USB plugs

unsigned char current_dc1 = 100, current_dc2 = 100;

typedef enum
{
	APP_STATE_INIT=0,
	APP_STATE_SERVICE_TASKS,
} APP_STATES;

typedef struct
{
    APP_STATES state;
} APP_DATA;


/*
 * fxn: initMotor1and2()
 * -- set GPIO pins to put both motors forward
 * -- enable OC1 and OC2 in PWM mode
 * -- set original duty cycle to 0%
 * -- turn on TIMER2 
*/ 
void initMotor1and2();

// Set motor directions
void setMotor1Forward();
void setMotor2Forward();
void setMotor1Backward();
void setMotor2Backward();

// Set Duty Cycles (percent)
void setMotor1DC(unsigned char dc);
void setMotor2DC(unsigned char dc);


void APP_Initialize ( void );

void APP_Tasks( void );


#endif /* _APP_H */
