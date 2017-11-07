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

            posX -= delta_x;
            posY -= delta_y;
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
        orientation += 360;
}

// return direction to get to 90 or 270 degrees
unsigned int getNearestVertical(unsigned int o)
{
    unsigned int diff270 = abs(270 - o);
    unsigned int diff90  = abs(90  - o);

    int theta = (diff270 <= diff90) ? 270 : 90;
    return theta;
}

// void getOutOfCrashZone(int x, int y, int o, int *future_x, int *future_y, int *future_o)
// {
//     struct motorQueueData out;
//     if (y < CRASH_MARGIN_L)
//     {
//         // negative for right, positive for left
//         *future_o = getNearestVertical(o);
//         setOrientation(*future_o, o, 4);
//         // move forward or reverse
//         int dist = CRASH_MARGIN_L - y;
//         out.action = (*future_o == 90) ? FORWARD : REVERSE;
//         out.dist   = abs(dist);
//         out.speed  = 3;
//         *future_y = (*future_o == 90) ? (*future_y+abs(dist)) : (*future_y-abs(dist));
//         sendMsgToMotorQ(out);
//     }
//     else if (y > CRASH_MARGIN_H)
//     {
//         // negative for right, positive for left
//         *future_o = getNearestVertical(o);
//         setOrientation(*future_o, o, 4);
//         // move forward or reverse
//         int dist = y - CRASH_MARGIN_H;
//         out.action = (*future_o == 270) ? FORWARD : REVERSE;
//         out.dist   = abs(dist);
//         out.speed  = 3;
//         *future_y = (*future_o == 270) ? (*future_y+abs(dist)) : (*future_y-abs(dist));
//         sendMsgToMotorQ(out);
//     }
// }

void setOrientation(unsigned int goal, unsigned int current, unsigned char speed)
{
    if (goal == current)
        return;

    struct motorQueueData out;
    int turn = goal - current;

    if (turn > 180)
        turn = 180 - turn;
    if (turn < -180)
        turn = -180 - turn;

    out.action = (turn < 0) ? TURN_RIGHT : TURN_LEFT;
    out.dist   = abs(turn);
    out.speed  = speed;
    sendMsgToMotorQ(out);
}

void beginTask(unsigned int task)
{
    int future_x = posX;
    int future_y = posY;
    int future_o = orientation;

    // Generate the pickup instructions
    struct motorQueueData out;
    // ONE : am i in the crash margin?
    // if (posY < CRASH_MARGIN_L || posY > CRASH_MARGIN_H)
    //     getOutOfCrashZone(posX, posY, orientation, &future_x, &future_y, &future_o);
    // TWO : Adjust delta X
    int dX = PICKUP_ZONES[task] - (int)posX;
    if (dX != 0)
    {
        (dX < 0) ? setOrientation(180,orientation,3) : setOrientation(0,orientation,3);
        char buf[128];
        sprintf(buf,"{\"SEQ\":0,\"DELIV_NAV\":{\"DELTAX\":%i}}!", dX);
        commSendMsgToUartQueue(buf);
        (dX < 0) ? (future_o = 180) : (future_o = 0);
        future_x += dX;
        out.action = FORWARD;
        out.dist   = abs(dX);
        out.speed  = 4;
        sendMsgToMotorQ(out);
    }
    // Three : Adjust delta Y
    int dY = PROD_Y - (int)posY;
    if (dY != 0)
    {
        (dY < 0) ? setOrientation(270,future_o,3) : setOrientation(90,future_o,3);
        (dX < 0) ? (future_o = 270) : (future_o = 90);
        future_y += dY;
        out.action = FORWARD;
        out.dist   = abs(dY);
        out.speed  = 4;
        sendMsgToMotorQ(out);
    }
    out.action = STOP;
    sendMsgToMotorQ(out);
}

void generateDeliveryDirs(unsigned int zone)
{
    int future_x = posX;
    int future_y = posY;
    int future_o = orientation;

    task_done = false;
    struct motorQueueData out;
    int dX = DROP_ZONES[zone] - posX;
    if (dX != 0)
    {
        (dX < 0) ? setOrientation(180,future_o,3) : setOrientation(0,future_o,3);
        (dX < 0) ? (future_o = 180) : (future_o = 0);
        future_x += dX;
        out.action = FORWARD;
        out.dist   = abs(dX);
        out.speed  = 4;
        sendMsgToMotorQ(out);
    }
    // Three : Adjust delta Y
    int dY = DELIV_Y - future_y;
    if (dY != 0) 
    {
        (dY < 0) ? setOrientation(270,future_o,3) : setOrientation(90,future_o,3);
        (dX < 0) ? (future_o = 270) : (future_o = 90);
        future_y += dY;
        out.action = FORWARD;
        out.dist   = abs(dY);
        out.speed  = 4;
        sendMsgToMotorQ(out);
        out.action = STOP;
        sendMsgToMotorQ(out);
    }
}

void handleIncomingMsg(struct navQueueData data)
{
    switch (data.type)
    {
        case ACTION:
        {
            if (navigationData.status == idle)
            {
                task_done = false;
                char buf[64];
                sprintf(buf, STR_ACK_ACT, outgoing_seq);
                commSendMsgToUartQueue(buf);
                
                struct motorQueueData out_m;
                out_m.action = data.a;
                out_m.dist   = data.b;
                out_m.speed  = data.c;
                sendMsgToMotorQ(out_m);
                out_m.action = STOP;
                sendMsgToMotorQ(out_m);
            }
            break;
        }
        case TASK:
        {
            if (navigationData.status == idle)
            {
                if (data.a > 2 || data.b > 2)
                    return; // bad incoming data
                navigationData.from = data.a;
                navigationData.to   = data.b;
                // set state to pickup
                navigationData.status = pickup;
                task_done = false;
                char buf[64];
                sprintf(buf, STR_ACK_TASK, outgoing_seq);
                commSendMsgToUartQueue(buf);
                beginTask(data.a);
            }
            break;
        }
        case POSITION:
        {
            char buf[64];
            sprintf(buf, STR_UPDATE_POS, outgoing_seq, data.a, data.b, data.c);
            commSendMsgToUartQueue(buf);
            sprintf(buf, STR_DBG, outgoing_seq, navigationData.status,navigationData.ir_dist, navigationData.magnet_is);
            commSendMsgToUartQueue(buf);
            break;
        }
        case SPEEDS:
        {
            navigationData.stopped = (data.c == 0 && data.d == 0 && task_done);
            char buf[128];
            if (navigationData.status == idle)
            {
                sprintf(buf, STR_IDLE, outgoing_seq);
                commSendMsgToUartQueue(buf);
            }
            break;
        }
        case POS_UPDATE:
        {
            updateLocation(data.a, data.b);
            break;
        }
        case DATA_REQ:
        {
            char buf[64];
            // Request Magnet state
            sprintf(buf, STR_DATA_REQ, outgoing_seq, navigationData.magnet_should_be, navigationData.ir_dist);
            commSendMsgToUartQueue(buf);
            break;
        }
        case DATA_UPDATE:
        {
            // update the magnet state
            navigationData.magnet_is = data.a;
            // make sure this shit makes sense...
            if (data.b > 0 && data.b < 10)
                navigationData.ir_dist = data.b;
            break;
        }
        default:
        {
            break;
        }
    }
}

void handleTaskState()
{
    switch (navigationData.status)
    {
        case idle:
        {
            generated = false;
            task_done = false;
            navigationData.stopped = false;
            break;
        }
        case pickup:
        {
            // use the IR data...
            if (navigationData.stopped && !navigationData.ir_used && motorQueuesAreEmpty())
            {
                struct motorQueueData out;
                out.action = FORWARD;
                out.dist   = navigationData.ir_dist;
                out.speed  = 2;
                sendMsgToMotorQ(out);
                out.action = STOP;
                sendMsgToMotorQ(out);
                navigationData.ir_used = true;
            }

            // check for done conditions
            if (navigationData.stopped && navigationData.ir_used && motorQueuesAreEmpty())
            {
                navigationData.status = magnet_on;
                navigationData.ir_used = false;
            }
            break;
        }
        case magnet_on:
        {
            navigationData.magnet_should_be = ON;
            // can i go to the next state ?
            if (navigationData.magnet_is == ON)
            {
                generated = false;
                navigationData.status = reverse1;
            }
            break;
        }
        case reverse1:
        {
            if (!generated)
            {   
                generated = true;
                struct motorQueueData out_m;
                out_m.action = REVERSE;
                out_m.dist   = 7;
                out_m.speed  = 3;
                sendMsgToMotorQ(out_m);
                out_m.action = STOP;
                sendMsgToMotorQ(out_m);
            }
            if (navigationData.stopped && motorQueuesAreEmpty())
            {
                navigationData.status = delivery;
                generated = false;
            }
            break;
        }
        case delivery:
        {
            // generate delivery directions
            if (!generated)
            {
                generated = true;
                generateDeliveryDirs(navigationData.to);
            }
            // Done Condition --> I've stopped moving
            if (navigationData.stopped && motorQueuesAreEmpty())
                navigationData.status = magnet_off;
            break;
        }
        case magnet_off:
        {
            navigationData.magnet_should_be = OFF;
            if (navigationData.magnet_is == OFF)
            {
                navigationData.status = reverse2;
                generated = false;
            }
            break;
        }
        case reverse2:
        {
            if (!generated)
            {
                generated = true;
                struct motorQueueData out_m;
                out_m.action = REVERSE;
                out_m.dist   = 10;
                out_m.speed  = 4;
                sendMsgToMotorQ(out_m);
                out_m.action = STOP;
                sendMsgToMotorQ(out_m);
            }
            if (navigationData.stopped && motorQueuesAreEmpty())
            {
                navigationData.status = idle;
                generated = false;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    // commSendMsgToUartQueue(buf);
}

void NAVIGATION_Initialize ( void )
{
    navigationData.state  = NAVIGATION_STATE_INIT;
    navigationData.status = idle;
    navigationData.magnet_should_be = OFF;
    navigationData.magnet_is        = OFF;
    navigationData.stopped = false;
    navigationData.ir_used = false;
    navigationData.ir_dist = 0;
    navigationData.from = 0xFF;
    navigationData.to   = 0xFF;
    task_done = false;

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
                handleIncomingMsg(rec);
                handleTaskState();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
