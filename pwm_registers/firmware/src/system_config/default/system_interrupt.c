#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"


    
    
void __ISR(_TIMER_4_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance0(void)
{
    current_dc1 = current_dc1 - 10;
    setMotor1DC(current_dc1);
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}


void __ISR(_TIMER_3_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance1(void)
{
    current_dc2 = current_dc2 - 10;
    setMotor2DC(current_dc2);
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
 /*******************************************************************************
 End of File
*/
