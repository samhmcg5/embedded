#include "motor_control.h"
#include "motor_globals.h"
#include "nav_globals.h"
#include "communication_globals.h"

MOTOR_CONTROL_DATA motor_controlData;

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
    
    // default motors to OFF
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
    float f_dc = (float)dc / 100.0;
    short int new_ocrs = TMR2_PERIOD * f_dc;
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

void setMotorR_Fwd()
{
    LATCCLR  = MOTOR_RIGHT_DIR_PIN;
}

void setMotorL_Fwd()
{
    LATGCLR  = MOTOR_LEFT_DIR_PIN;
}

void setMotorR_Bck()
{
    LATCCLR  = MOTOR_RIGHT_DIR_PIN;
    LATCSET  = MOTOR_RIGHT_DIR_PIN;
}

void setMotorL_Bck()
{
    LATGCLR  = MOTOR_LEFT_DIR_PIN;
    LATGSET  = MOTOR_LEFT_DIR_PIN;
}

void sendMsgToMotorQ(struct motorQueueData msg)
{
    xQueueSendToBack(motor_q, &msg, portMAX_DELAY);
}

void sendMsgToMotor_R(struct pwmQueueData msg)
{
    xQueueSendToBack(right_q, &msg, portMAX_DELAY);
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_4);

}

void sendMsgToMotor_L(struct pwmQueueData msg)
{
    xQueueSendToBack(left_q, &msg, portMAX_DELAY);
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_3);
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
    left_q = xQueueCreate(32, sizeof (struct pwmQueueData));
    // initialize the OCs and Timer2
    initMotors();
    
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    //DRV_TMR2_Start();
    setMotorR_DC(20);
    setMotorL_DC(20);

    
//    struct pwmQueueData data;
//    data.dc = 50;
//    data.dir = FORWARD;
//    data.dist = 20;
//    sendMsgToMotor_R(data);
//    sendMsgToMotor_L(data);
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
                motor_controlData.state = MOTOR_CONTROL_STATE_SERVICE_TASKS;
            }
            break;
        }

        case MOTOR_CONTROL_STATE_SERVICE_TASKS:
        {
            struct motorQueueData rec;
            if(xQueueReceive(motor_q, &rec, portMAX_DELAY))
            {
                char buf[16];
                sprintf(buf, "MOTOR  %i\n\r", rec.dist);
                commSendMsgToUartQueue(buf);
                
//                struct pwmQueueData data;
//                data.dc = 50;
//                data.dir = FORWARD;
//                data.dist = rec.dist;
//                
//                sendMsgToMotor_R(data);
//                sendMsgToMotor_L(data);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}
