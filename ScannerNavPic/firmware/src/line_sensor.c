#include "line_sensor.h"
#include "communication.h"
#include "communication_globals.h"
#include "motor_control.h"
#include "motor_globals.h"

LINE_SENSOR_DATA line_sensorData;

void sendMsgToSensorQFromISR(struct sensorQueueData msg)
{
    xQueueSendToBackFromISR(sensor_q, &msg, NULL);
}

void LINE_SENSOR_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    line_sensorData.state = LINE_SENSOR_STATE_INIT;

}

void sensorHandleIncomingMsg(struct sensorQueueData data)
{
    struct motorQueueData out;
    out.type = SENSOR;
    out.action = 0;
    out.dist = 5;
    out.speed = 10;
    sendMsgToMotorQ(out);
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
            sensor_q = xQueueCreate(32, sizeof (struct sensorQueueData));
        
            if (appInitialized)
            {
            
                line_sensorData.state = LINE_SENSOR_STATE_SERVICE_TASKS;
            }
            break;
        }

        case LINE_SENSOR_STATE_SERVICE_TASKS:
        {
            struct sensorQueueData rec;
            if(xQueueReceive(sensor_q, &rec, portMAX_DELAY))
            {
                //dbgOutputLoc(MOTOR_THREAD_RECVD);
                
                sensorHandleIncomingMsg(rec);
            }
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

