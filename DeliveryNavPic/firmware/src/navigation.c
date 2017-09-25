#include "navigation.h"
#include "nav_globals.h"
#include "communication_globals.h"
#include "motor_globals.h"

NAVIGATION_DATA navigationData;

void sendMsgToNavQ(struct navQueueData msg)
{
    xQueueSendToBack(nav_q, &msg, portMAX_DELAY);
}

void sendMsgToNavQFromISR(struct navQueueData msg)
{
    xQueueSendToBackFromISR(nav_q, &msg, NULL);
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
                
                // pass directly to the motor queue
                if (rec.type == ACTION)
                {
                    struct motorQueueData out_m;
                    out_m.action = rec.a;
                    out_m.dist   = rec.b;
                    out_m.speed  = rec.c;
                    sendMsgToMotorQ(out_m);
                }
                // recvd from motor, pass to UART
                else if (rec.type == SPEEDS)
                {
                    char buf[128];
                    //if (rec.c > 0 || rec.d > 0)
                    sprintf(buf, "{seq:%i, right_dir:%i, left_dir:%i, right_speed:%u, left_speed:%u}!", 
                            outgoing_seq, rec.a, rec.b, rec.c, rec.d);
                    //else
                    //    sprintf(buf, "{\"seq\":%i, \"status\":\"idle\"}!", outgoing_seq);
                    commSendMsgToUartQueue(buf);
                }
                else if (rec.type == POSITION)
                {
                    char buf[64];
                    sprintf(buf, "{seq:%i, x:%i, y:%i}!", outgoing_seq, rec.a, rec.b);
                    commSendMsgToUartQueue(buf);
                }
                else if (rec.type == STATUS)
                {
                    // TODO
                }
                else // figure out path to the object, convert to action items
                {
                    // TODO
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
