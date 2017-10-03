#ifndef MOTOR_GLOBALS_H
#define	MOTOR_GLOBALS_H

// PIN DIRECTIONS FOR MOTORS
#define FORWARD     0 
#define REVERSE     1
#define TURN_LEFT   2
#define TURN_RIGHT  3
#define STOP        4

#define TICKS_PER_CM    74

// targets ticks per tenth second
#define SPEED_0         0      // dc = 0
#define SPEED_1         15      // dc = 15
#define SPEED_2         32      // dc = 20
#define SPEED_3         40      // dc = 30
#define SPEED_4         45      // dc = 50
#define SPEED_5         50      // dc = 95

unsigned char speeds[] = {SPEED_0, SPEED_1, SPEED_2, SPEED_3, SPEED_4, SPEED_5};

unsigned char calculateKP(unsigned char speed, unsigned char dir);

// for motor selection
#define LEFT  0
#define RIGHT 1

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
    char speed;
    char action;
};

void sendMsgToMotor_R(struct pwmQueueData);
void sendMsgToMotor_L(struct pwmQueueData);

unsigned char getMotorR_DC();
unsigned char getMotorL_DC();

unsigned char getMotorR_Dir();
unsigned char getMotorL_Dir();
unsigned char getMotorAction();
unsigned char getSpeedSettingR();
unsigned char getSpeedSettingL();

#endif	/* MOTOR_GLOBALS_H */

