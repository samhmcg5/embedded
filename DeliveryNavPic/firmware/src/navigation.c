#include "navigation.h"
#include "nav_globals.h"
#include "communication_globals.h"
#include "motor_globals.h"

NAVIGATION_DATA navigationData;

void sendMsgToNavQ(struct navQueueData msg)
{
    xQueueSendToBack(nav_q, &msg, portMAX_DELAY);
}

void NAVIGATION_Initialize ( void )
{
    navigationData.state = NAVIGATION_STATE_INIT;
    nav_q = xQueueCreate(32, sizeof (struct navQueueData));
}

void NAVIGATION_Tasks ( void )
{
    switch ( navigationData.state )
    {
        case NAVIGATION_STATE_INIT:
        {
            bool appInitialized = true;
            if (appInitialized)
            {
                navigationData.state = NAVIGATION_STATE_SERVICE_TASKS;
            }
            break;
        }

        case NAVIGATION_STATE_SERVICE_TASKS:
        {
            dbgOutputLoc(NAV_THREAD_WAIT);
            
            struct navQueueData rec;
            if(xQueueReceive(nav_q, &rec, portMAX_DELAY))
            {
                dbgOutputLoc(NAV_THREAD_RECVD);
                
                struct motorQueueData out_m;
                // pass directly to the motor queue
                if (rec.type == ACTION)
                {
                    out_m.action = rec.a;
                    out_m.dist = rec.b;
                    out_m.speed = rec.c;
                }
                else if (rec.type == SPEEDS)
                {
                    // TODO
                }
                else if (rec.type == POSITION)
                {
                    // TODO
                }
                else // figure out path to the object
                {
                    // TODO
                }
                sendMsgToMotorQ(out_m);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
