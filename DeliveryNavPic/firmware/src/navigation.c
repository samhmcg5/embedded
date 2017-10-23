#include "navigation.h"
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

void updateLocation(unsigned int cm, unsigned char action)
{
    switch(action)
    {
        case FORWARD:
        {
            float delta_x = cm * cos(convert * orientation);
            float delta_y = cm * sin(convert * orientation);

            posX += delta_x;
            posY += delta_y;
            break;
        }
        case REVERSE:
        {
            float delta_x = cm * cos(convert * orientation);
            float delta_y = cm * sin(convert * orientation);

            posX -= (int)delta_x;
            posY -= (int)delta_y;
            break;
        }
        case TURN_LEFT:
        {
            orientation += cm;
            break;
        }
        case TURN_RIGHT:
        {
            orientation -= cm;
            break;
        }
        case STOP:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    // check the range of the orientation
    while (orientation > 360)
        orientation -= 360;
    while (orientation < 0)
        orientation = 360 + orientation;
}

void handleIncomingMsg(struct navQueueData data)
{
    switch (data.type)
    {
        case ACTION:
        {
            struct motorQueueData out_m;
            out_m.action = data.a;
            out_m.dist   = data.b;
            out_m.speed  = data.c;
            sendMsgToMotorQ(out_m);
            break;
        }
        case TASK:
        {
            // TODO
            break;
        }
        case POSITION:
        {
            char buf[64];
            sprintf(buf, STR_UPDATE_POS, outgoing_seq, data.a, data.b, data.c);
            commSendMsgToUartQueue(buf);
            break;
        }
        case SPEEDS:
        {
            char buf[128];
            sprintf(buf, STR_UPDATE_SPEED, outgoing_seq, data.a, data.b, data.c, data.d);
            commSendMsgToUartQueue(buf);
            if (data.c == 0 && data.d == 0 && navigationData.status == moving)
            {
                sprintf(buf, STR_IDLE, outgoing_seq);
                commSendMsgToUartQueue(buf);
                navigationData.status = idle;
            }
            else if ( (data.c != 0 || data.d != 0) && navigationData.status == idle)
            {
                navigationData.status = moving;
            }
            break;
        }
        case POS_UPDATE:
        {
            updateLocation(data.a, data.b);
            break;
        }
        default:
        {
            break;
        }
    }
}

void NAVIGATION_Initialize ( void )
{
    navigationData.state = NAVIGATION_STATE_INIT;
    navigationData.status = idle;
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
                handleIncomingMsg(rec);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
