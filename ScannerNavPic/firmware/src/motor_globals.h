#ifndef MOTOR_GLOBALS_H
#define	MOTOR_GLOBALS_H

// PIN DIRECTIONS FOR MOTORS
#define FORWARD     0 // also an action
#define BACKWARD    1 // also an action...
// ACTIONS
#define TURN_LEFT   2
#define TURN_RIGHT  3
#define STOP        4

// send to motor
#define ACTION      0
#define TASK        1
// send to UART, coming from motor thread
#define SPEEDS      2
#define POSITION    3
#define SENSOR      4

// data inside Nav's incoming queue
struct motorQueueData
{
    char type;
    char action;
    char dist;
    char speed;
};

void sendMsgToMotorQ(struct motorQueueData msg);
void sendMsgToMotorQFromISR(struct motorQueueData msg);

struct pwmQueueData
{
    char dc;
    char dir;
    char dist;
};

void sendMsgToMotor_R(struct pwmQueueData);
void sendMsgToMotor_L(struct pwmQueueData);

unsigned char getMotorR_DC();
unsigned char getMotorL_DC();
unsigned char getMotorR_Dir();
unsigned char getMotorL_Dir();

#endif	/* MOTOR_GLOBALS_H */

