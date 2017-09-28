#include <stdio.h>
#include "app.h"
#include "debug.h"


APP_DATA appData;

void initMotor1and2()
{
    // default directions to forward
    // Motor 1 direction 0x4000
    TRISCCLR = MOTOR1_DIR_PIN;
    ODCCCLR  = MOTOR1_DIR_PIN;
    LATCCLR  = MOTOR1_DIR_PIN;

    // motor 2 direction 0x0002
    TRISGCLR = MOTOR2_DIR_PIN;
    ODCGCLR  = MOTOR2_DIR_PIN;
    LATGCLR  = MOTOR2_DIR_PIN;
    
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

void setMotor1Forward()
{
    LATCCLR  = MOTOR1_DIR_PIN;
}

void setMotor2Forward()
{
    LATGCLR  = MOTOR2_DIR_PIN;
}

void setMotor1Backward()
{
    LATCCLR  = MOTOR1_DIR_PIN;
    LATCSET  = MOTOR1_DIR_PIN;
}

void setMotor2Backward()
{
    LATGCLR  = MOTOR2_DIR_PIN;
    LATGSET  = MOTOR2_DIR_PIN;
}

void setMotor1DC(unsigned char dc)
{
    float f_dc = (float)dc / 100.0;
    short int new_ocrs = TMR2_PERIOD * f_dc;
    OC1RS = new_ocrs;
}

void setMotor2DC(unsigned char dc)
{
    float f_dc = (float)dc / 100.0;
    short int new_ocrs = TMR2_PERIOD * f_dc;
    OC2RS = new_ocrs;
}

// add a msg to the q
int sendMsgToQFromISR(struct queueData msg)
{
    xQueueSendFromISR(encoder_q, (void*)&msg, NULL);
    return 0;
}

// recv a message from the q
struct queueData recvFromQ()
{
    struct queueData recv;
    if (encoder_q != 0)
    {
       xQueueReceive(encoder_q, &recv, portMAX_DELAY);
    }
    return recv;
}

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    encoder_q = xQueueCreate(64, sizeof(struct queueData));
    
    initMotor1and2();
    
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
    
    setMotor1DC(20);
    setMotor2DC(20);
}

void APP_Tasks ( void )
{
    switch ( appData.state )
    {
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            if (appInitialized)
            {
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            while (1)
            {
                struct queueData msg = recvFromQ();
//                char buf[12];
//                sprintf(buf, "%i\t%i\n\r", msg.motor1, msg.motor2);
//                writeUARTString(buf,12);
                dbgOutputVal(msg.motor1);
                dbgOutputVal(msg.motor2);
            }
            
            break;
        }

        default:
        {
            break;
        }
    }
}

