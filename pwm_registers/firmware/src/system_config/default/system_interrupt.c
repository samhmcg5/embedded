#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"


int cm_count = 0;
char state = 0;
    
    
void __ISR(_TIMER_4_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance0(void)
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
        }
        else
        { 
            state = 0;
            setMotor1Forward();
            setMotor2Forward();
        }
    }
    cm_count++;
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}


void __ISR(_TIMER_3_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance1(void)
{
    //current_dc2 = current_dc2 - 10;
    //setMotor2DC(current_dc2);
    //setMotor2DC(0);
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
 /*******************************************************************************
 End of File
*/
