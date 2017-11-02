#include "line_sensor.h"
#include "communication_globals.h"
#include "motor_globals.h"
#include "i2c.h"

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
    if(sensor_enable) {
        if(data.front_sensor > 75) {
                struct motorQueueData out;
                out.type = ACTION;    
                out.action = 2;
                out.dist = 5;
                out.speed = 2;
                sendMsgToMotorQ(out);

                char buf[64];
                sprintf(buf, "{\"SEQ\":%i, \"SCAN_NAV\": { \"SENSOR\": \"FRONT\", \"VALUE\": %i, \"TYPE\": \"LEFT CORRECTION\" }}!", outgoing_seq, data.front_sensor);
                commSendMsgToUartQueue(buf);
        }
        else if(data.rear_sensor > 75) {
                struct motorQueueData out;
                out.type = ACTION;    
                out.action = 3;
                out.dist = 5;
                out.speed = 2;
                sendMsgToMotorQ(out);

                char buf[64];
                sprintf(buf, "{\"SEQ\":%i, \"SCAN_NAV\": { \"SENSOR\": \"REAR\", \"VALUE\": %i, \"TYPE\": \"RIGHT CORRECTION\" }}!", outgoing_seq, data.rear_sensor);
                commSendMsgToUartQueue(buf);
        }
    }
    
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
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


