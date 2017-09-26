#include "motor_control.h"
#include "motor_globals.h"
#include "nav_globals.h"
#include "communication_globals.h"

MOTOR_CONTROL_DATA motor_controlData;

void generateActionItems(struct motorQueueData data, struct pwmQueueData * left, struct pwmQueueData * right)
{
    switch (data.action)
    {
        case FORWARD:
            left->dir = FORWARD;
            left->dc = data.speed;
            left->dist = data.dist;
            right->dir = FORWARD;
            right->dc = data.speed;
            right->dist = data.dist;
            break;
        case BACKWARD:
            left->dir = BACKWARD;
            left->dc = data.speed;
            left->dist = data.dist;
            right->dir = BACKWARD;
            right->dc = data.speed;
            right->dist = data.dist;
            break;  
        case TURN_LEFT:
            left->dir = BACKWARD;
            left->dc = data.speed;
            left->dist = data.dist;
            right->dir = FORWARD;
            right->dc = data.speed;
            right->dist = data.dist;
            break;
        case TURN_RIGHT:
            left->dir = FORWARD;
            left->dc = data.speed;
            left->dist = data.dist;
            right->dir = BACKWARD;
            right->dc = data.speed;
            right->dist = data.dist;
            break;
        case STOP:
            left->dir = 0;
            left->dc = 0;
            left->dist = 0;
            right->dir = 0;
            right->dc = 0;
            right->dist = 0;
            break;
        default:
            // handle and error
            break;
    }
}

void initMotors()
{
    // default directions to forward
    // Motor 1 direction 0x4000
    TRISCCLR = MOTOR_RIGHT_DIR_PIN;
    ODCCCLR  = MOTOR_RIGHT_DIR_PIN;
    LATCCLR  = MOTOR_RIGHT_DIR_PIN;

    // motor 2 direction 0x0002
    TRISGCLR = MOTOR_LEFT_DIR_PIN;
    ODCGCLR  = MOTOR_LEFT_DIR_PIN;
    LATGCLR  = MOTOR_LEFT_DIR_PIN;
    
    T2CONSET = 0x0070;   // set for 256 prescale
    PR2 = TMR2_PERIOD;  // set the period to 3125
    
    // initialize motors to OFF (duty cycle = 0%)
    OC1CON = 0x0000;    // turn off while setting up
    OC1R = 0x0000;      // primary compare register
    OC1RS = 0x0000;     // secondary compare register
    OC1CON = 0x0006;    // configure for PWM mode
    
    OC2CON = 0x0000;    // turn off while setting up
    OC2R = 0x0000;      // primary compare register
    OC2RS = 0x0000;     // secondary compare register
    OC2CON = 0x0006;    // configure for PWM mode
    
    T2CONSET = 0x8000;  // enable timer 2
    OC1CONSET = 0x8000; // enable OC1
    OC2CONSET = 0x8000; // enable OC2
}

void setMotorR_DC(unsigned char dc)
{
    // convert duty cycle arg to a float between 0 and 1
    float f_dc = (float)dc / 100.0;
    short int new_ocrs = TMR2_PERIOD * f_dc;
    // set the output compare register
    OC1RS = new_ocrs;
    
    motor_controlData.dcR = dc;
}

void setMotorL_DC(unsigned char dc)
{
    float f_dc = (float)dc / 100.0;
    short int new_ocrs = TMR2_PERIOD * f_dc;
    OC2RS = new_ocrs;
    
    motor_controlData.dcL = dc;
}

unsigned char getMotorR_DC()
{
    return motor_controlData.dcR;
}

unsigned char getMotorL_DC()
{
    return motor_controlData.dcL;
}

unsigned char getMotorR_Dir()
{
    return motor_controlData.dirR;
}

unsigned char getMotorL_Dir()
{
    return motor_controlData.dirL;
}

void setMotorR_Fwd()
{
    LATCCLR  = MOTOR_RIGHT_DIR_PIN;
    motor_controlData.dirR = FORWARD;
}

void setMotorL_Fwd()
{
    LATGCLR  = MOTOR_LEFT_DIR_PIN;
    motor_controlData.dirL = FORWARD;
}

void setMotorR_Bck()
{
    LATCCLR  = MOTOR_RIGHT_DIR_PIN;
    LATCSET  = MOTOR_RIGHT_DIR_PIN;
    
    motor_controlData.dirR = BACKWARD;
}

void setMotorL_Bck()
{
    LATGCLR  = MOTOR_LEFT_DIR_PIN;
    LATGSET  = MOTOR_LEFT_DIR_PIN;
    
    motor_controlData.dirL = BACKWARD;
}

void sendMsgToMotorQ(struct motorQueueData msg)
{
    xQueueSendToBack(motor_q, &msg, portMAX_DELAY);
}

void sendMsgToMotor_R(struct pwmQueueData msg)
{
    dbgOutputLoc(MOTOR_THR_SEND_TO_Q_R);
    
    if (getMotorR_DC() == 0)
    {
        setMotorR_DC(msg.dc);
        goalR = msg.dist * 60;
        if (msg.dir == FORWARD)
            setMotorR_Fwd();
        else
            setMotorR_Bck();   
    }
    else
    {
     xQueueSendToBack(right_q, &msg, portMAX_DELAY);
    }
}

void sendMsgToMotor_L(struct pwmQueueData msg)
{
    dbgOutputLoc(MOTOR_THR_SEND_TO_Q_L);

    if (getMotorL_DC() == 0)
    {
        setMotorL_DC(msg.dc);
        goalL = msg.dist * 60;
        if (msg.dir == FORWARD)
            setMotorL_Fwd();
        else
            setMotorL_Bck();   
    }
    else
    {
     xQueueSendToBack(left_q, &msg, portMAX_DELAY);
    }
}

void motorR_recvQInISR(struct pwmQueueData* msg) 
{
    xQueueReceiveFromISR(right_q, msg, NULL);
}

void motorL_recvQInISR(struct pwmQueueData* msg) 
{
    xQueueReceiveFromISR(left_q, msg, NULL);
}

bool rightQIsEmpty()
{
    return xQueueIsQueueEmptyFromISR(right_q);
}

bool leftQIsEmpty()
{
    return xQueueIsQueueEmptyFromISR(left_q);
}

void MOTOR_CONTROL_Initialize ( void )
{
    motor_controlData.state = MOTOR_CONTROL_STATE_INIT;
    // incoming queue
    motor_q = xQueueCreate(32, sizeof (struct motorQueueData));
    right_q = xQueueCreate(32, sizeof (struct pwmQueueData));
    left_q  = xQueueCreate(32, sizeof (struct pwmQueueData));
    // initialize the OCs and Timer2
    initMotors();
    
    // start the encoder counters
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
}

void MOTOR_CONTROL_Tasks ( void )
{
    switch ( motor_controlData.state )
    {
        case MOTOR_CONTROL_STATE_INIT:
        {
            bool appInitialized = true;
            if (appInitialized)
            {
                motor_controlData.state = MOTOR_CONTROL_HANDLE_INCOMING;
            }
            break;
        }

        case MOTOR_CONTROL_HANDLE_INCOMING:
        {
            dbgOutputLoc(MOTOR_THREAD_WAIT);
            
            struct motorQueueData rec;
            if(xQueueReceive(motor_q, &rec, portMAX_DELAY))
            {
                dbgOutputLoc(MOTOR_THREAD_RECVD);
                
                struct pwmQueueData left, right;
                generateActionItems(rec, &left, &right);
                
                sendMsgToMotor_L(left);
                sendMsgToMotor_R(right);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}
