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
            // TODO
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
unsigned int getNearestVertical()
{
    unsigned int diff270 = abs(270 - orientation);
    unsigned int diff90  = abs(90  - orientation);

    int theta = (diff270 <= diff90) ? 270 : 90;
    return theta;
}

void getOutOfCrashZone()
{
    struct motorQueueData out;
    if (posY < CRASH_MARGIN_L)
    {
        // negative for right, positive for left
        setOrientation(getNearestVertical(), 4);
        // move forward or reverse
        int dist = CRASH_MARGIN_L - posY;
        out.action = (getNearestVertical() == 90) ? FORWARD : REVERSE;
        out.dist   = dist;
        out.speed  = 3;
        sendMsgToMotorQ(out);
    }
    else if (posY > CRASH_MARGIN_H)
    {
        // negative for right, positive for left
        setOrientation(getNearestVertical(), 4);
        // move forward or reverse
        int dist = posY - CRASH_MARGIN_H;
        out.action = (getNearestVertical() == 270) ? FORWARD : REVERSE;
        out.dist   = dist;
        out.speed  = 3;
        sendMsgToMotorQ(out);
    }
}

void setOrientation(unsigned int theta, unsigned char speed)
{
    if (theta == orientation)
        return;

    struct motorQueueData out;
    int turn   = theta - orientation;
    if (turn > 180)
        turn = 180 - turn;
    out.action = (turn <= 0) ? TURN_RIGHT : TURN_LEFT;
    out.dist   = abs(turn);
    out.speed  = speed;
    sendMsgToMotorQ(out);
}

int getPickupDX(unsigned int zone)
{
    if (zone == 0)      {   return RED_X - posX;    }
    else if (zone == 1) {   return GREEN_X - posX;  }
    else if (zone == 2) {   return BLUE_X - posX;   }
    else                {   return 0;   }
}

void beginTask(struct navQueueData task)
{
    // set state to pickup
    navigationData.status = pickup;
    // Generate the pickup instructions
    struct motorQueueData out;
    // ONE : am i in the crash margin?
    if (posY < CRASH_MARGIN_L || posY > CRASH_MARGIN_H)
        getOutOfCrashZone();
    // TWO : Adjust delta X
    int dX = getPickupDX(task.a);
    (dX < 0) ? setOrientation(180,3) : setOrientation(0,3);
    out.action = FORWARD;
    out.dist   = abs(dX);
    out.speed  = 4;
    sendMsgToMotorQ(out);
    // Three : Adjust delta Y
    int dY = PROD_Y - posY;
    (dY < 0) ? setOrientation(270,3) : setOrientation(90,3);
    out.action = FORWARD;
    out.dist   = abs(dY);
    out.speed  = 4;
    sendMsgToMotorQ(out);
    // Four : check orientation for 270 degrees
    // specific to pickup...
    // if (orientation != 270)
    //     setOrientation(270,2);
}

void handleIncomingMsg(struct navQueueData data)
{
    switch (data.type)
    {
        case ACTION:
        {
            if (navigationData.status == idle)
            {
                char buf[64];
                sprintf(buf, STR_ACK_ACT, outgoing_seq);
                commSendMsgToUartQueue(buf);
                
                struct motorQueueData out_m;
                out_m.action = data.a;
                out_m.dist   = data.b;
                out_m.speed  = data.c;
                sendMsgToMotorQ(out_m);
            }
            break;
        }
        case TASK:
        {
            if (navigationData.status == idle)
            {
                char buf[64];
                sprintf(buf, STR_ACK_TASK, outgoing_seq);
                commSendMsgToUartQueue(buf);
                
                beginTask(data);
            }
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
            if (navigationData.status == idle && data.c == 0 && data.d == 0)
            {
                sprintf(buf, STR_IDLE, outgoing_seq);
                commSendMsgToUartQueue(buf);
                // navigationData.status = idle;
            }
            // else if ( data.c != 0 || data.d != 0 )
            // {
            //     sprintf(buf, STR_EXECUTING, outgoing_seq);
            //     commSendMsgToUartQueue(buf);
            //     // navigationData.status = moving;
            // }
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
            sprintf(buf, STR_MAG_REQ, outgoing_seq, navigationData.magnet_should_be);
            commSendMsgToUartQueue(buf);
            // Request IR data
            sprintf(buf, STR_IR_REQ);
            commSendMsgToUartQueue(buf);
            break;
        }
        case MAG_UPDATE:
        {
            navigationData.magnet_is = data.a;
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
            // Do nothing, this is handled by incoming message
            break;
        }
        case pickup:
        {
            // test the done condition for getting to the pickup spot
            /*
            Done Conditions:
                1. I have stopped moving
                2. I have read the IR data and acted upon it
            */
            break;
        }
        case magnet_on:
        {
            navigationData.magnet_should_be = ON;
            // can i go to the next state ?
            if (navigationData.magnet_is == ON)
                navigationData.status = delivery;
            break;
        }
        case delivery:
        {
            // generate delivery directions
            // Done Condition --> I've stopped moving
            break;
        }
        case magnet_off:
        {
            navigationData.magnet_should_be = OFF;
            if (navigationData.magnet_is == OFF)
                navigationData.status = reverse;
            break;
        }
        case reverse:
        {
            struct motorQueueData out_m;
            out_m.action = REVERSE;
            out_m.dist   = 10;
            out_m.speed  = 4;
            sendMsgToMotorQ(out_m);
            navigationData.status = idle;
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
    navigationData.magnet_should_be = OFF;
    navigationData.magnet_is = OFF;
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
