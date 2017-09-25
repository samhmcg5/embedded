#ifndef MOTOR_GLOBALS_H
#define	MOTOR_GLOBALS_H

// PIN DIRECTIONS FOR MOTORS
#define FORWARD     0 // also an action
#define BACKWARD    1 // also an action...
// ACTIONS
#define TURN_LEFT   2
#define TURN_RIGHT  3
#define STOP        4

// data inside Nav's incoming queue
struct motorQueueData
{
    char action;
    char dist;
    char speed;
};

void sendMsgToMotorQ(struct motorQueueData msg);

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

#endif	/* MOTOR_GLOBALS_H */

