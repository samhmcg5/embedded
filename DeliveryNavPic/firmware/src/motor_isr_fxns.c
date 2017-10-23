#include "motor_globals.h"

// void readTickData()
// {
//     /* Get the current value from the timer registers */
//     short int ticksL = DRV_TMR0_CounterValueGet();
//     short int ticksR = DRV_TMR1_CounterValueGet();

//     /* Clear the counter registers */
//     DRV_TMR0_CounterClear();
//     DRV_TMR1_CounterClear();

//     total_ticksL += ticksL;
//     total_ticksR += ticksR;

//     /* increment the distnace traveled per motor in ticks*/
//     distL        += ticksL;
//     distR        += ticksR;
// }


/*
    if (prev_action != getMotorAction())
        cm = 0;
    cm += ticksL
    if (getMotorAction() == FORWARD && cm >= TICKS_PER_CM)
    {

    }
    else if (getMotorAction() == REVERSE && cm >= TICKS_PER_CM)
    {

    }
    else if (getMotorAction() == TURN_LEFT)
    {

    }
    else if (getMotorAction() == TURN_RIGHT)
    {

    }

*/
