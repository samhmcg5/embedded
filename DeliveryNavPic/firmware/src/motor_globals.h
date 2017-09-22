#ifndef MOTOR_GLOBALS_H
#define	MOTOR_GLOBALS_H

#define FORWARD  0
#define BACKWARD 1

// data inside Nav's incoming queue
struct motorQueueData
{
    char dist;
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

#endif	/* MOTOR_GLOBALS_H */

