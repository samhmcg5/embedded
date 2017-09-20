#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"


int cm_count = 0;
char timer5_count = 0;
unsigned char ticks1 = 0, ticks2 = 0;
char state = 0;
    
    
void IntHandlerDrvTmrInstance0(void)
{
    //current_dc1 = current_dc1 - 10;
    //setMotor1DC(current_dc1);
    //setMotor1DC(0);
    
    if (cm_count % 10 == 0)
    {
        if (state == 0)
        {
            state = 1;
            setMotor1Backward();
            setMotor2Backward();
            setMotor1DC(70);
            setMotor2DC(70);
        }
        else
        { 
            state = 0;
            setMotor1Forward();
            setMotor2Forward();
            setMotor1DC(15);
            setMotor2DC(15);
        }
    }
    cm_count++;
    ticks1 = ticks1 + 60;
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}
void IntHandlerDrvTmrInstance1(void)
{
    //current_dc2 = current_dc2 - 10;
    //setMotor2DC(current_dc2);
    //setMotor2DC(0);
    ticks2 = ticks2 + 60;
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}

void IntHandlerDrvTmrInstance2(void)
{
    if (timer5_count % 10 == 0)
    {
        struct queueData msg;
        msg.motor1 = ticks1;
        msg.motor2 = ticks2;
        
        sendMsgToQFromISR(msg);
        
        ticks1 = 0;
        ticks2 = 0;
    }
    timer5_count++;
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}
 