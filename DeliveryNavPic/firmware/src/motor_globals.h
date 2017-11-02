#ifndef MOTOR_GLOBALS_H
#define	MOTOR_GLOBALS_H

// PIN DIRECTIONS FOR MOTORS
#define FORWARD     0
#define REVERSE     1
#define TURN_LEFT   2
#define TURN_RIGHT  3
#define STOP        4

#define TICKS_PER_CM        74
#define TP_DEGREE_L         7.32
#define TP_DEGREE_R         7.33

#define SPEED_0         0
#define SPEED_1         470
#define SPEED_2         625
#define SPEED_3         940
#define SPEED_4         1560
#define SPEED_5         2815

unsigned int speeds[] = {SPEED_0, SPEED_1, SPEED_2, SPEED_3, SPEED_4, SPEED_5};

// for motor selection
#define LEFT  0
#define RIGHT 1

// for error correction
#define KP   5
#define KI   20
int integral = 0;

// for motion control
unsigned int distR = 0; // measured in ticks
unsigned int distL = 0;
unsigned int goalL = 0;
unsigned int goalR = 0;
unsigned int total_ticksL = 0;
unsigned int total_ticksR = 0;
unsigned int prev_cm = 0;

// data inside Nav's incoming queue
struct motorQueueData
{
    char action;
    unsigned int dist;
    char speed;
};

void sendMsgToMotorQ(struct motorQueueData msg);

struct pwmQueueData
{
    unsigned int dc;
    unsigned int dist;
    char dir;
    char action;
};

void sendMsgToMotor_R(struct pwmQueueData);
void sendMsgToMotor_L(struct pwmQueueData);

unsigned int getMotorR_DC();
unsigned int getMotorL_DC();

unsigned char getMotorR_Dir();
unsigned char getMotorL_Dir();
unsigned char getMotorAction();

#endif	/* MOTOR_GLOBALS_H */

