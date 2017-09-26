#include "line_sensor.h"
#include "line_sensor_globals.h"

LINE_SENSOR_DATA line_sensorData;

void LINE_SENSOR_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    line_sensorData.state = LINE_SENSOR_STATE_INIT;
  
}

void LINE_SENSOR_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( line_sensorData.state )
    {
        /* Application's initial state. */
        case LINE_SENSOR_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                line_sensorData.state = LINE_SENSOR_STATE_SERVICE_TASKS;
            }
            break;
        }

        case LINE_SENSOR_STATE_SERVICE_TASKS:
        {
            struct  motorQueueData out;
            out.action = FORWARD;
            sendMsgToMotorQ(out);
            break;
        }

        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

