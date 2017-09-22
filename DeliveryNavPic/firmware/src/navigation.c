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
            struct navQueueData rec;
            if(xQueueReceive(nav_q, &rec, portMAX_DELAY))
            {
                char buf[64];
                sprintf(buf, "NAV %i %i %i %i\n\r", rec.msg, rec.x, rec.y, rec.color);
                commSendMsgToUartQueue(buf);
    
                struct motorQueueData out_m;
                out_m.dist = rec.msg;
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
