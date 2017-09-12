#include "communication.h"

COMMUNICATION_DATA communicationData;

// add a msg to the q
int sendMsgToQFromISR(unsigned int msg)
{
    xQueueSendFromISR(q, (void*)&msg, NULL);
    return 0;
}

// for 4 to 30 cm
int voltsToCm(unsigned int volt)
{
    volt = volt * 50;
    volt = volt + 3;
    return 519/volt;
}

// recv a message from the q
unsigned int recvFromQ()
{
    unsigned int recv;
    if (q != 0)
    {
       xQueueReceive(q, &recv, portMAX_DELAY);
    }
    return recv;
}

void COMMUNICATION_Initialize ( void )
{
    communicationData.state = COMMUNICATION_STATE_INIT;
}


void COMMUNICATION_Tasks ( void )
{
    switch ( communicationData.state )
    {
        case COMMUNICATION_STATE_INIT:
        {
            tmr_message.msg_state = TMR_MESSAGE_STATE_T;
            bool appInitialized = true;
            // create the q
            q = xQueueCreate(16, sizeof(unsigned int) );
            if (q == 0)
            {
                // failed to create
                halt(DBG_ERROR_QUEUE_FAILED_TO_CREATE);
            }
            
            if (appInitialized)
            {
                communicationData.state = COMMUNICATION_STATE_RUNNING;
            }
            break;
        }
        case COMMUNICATION_STATE_RUNNING:
        {
            DRV_TMR0_Start();
            DRV_ADC_Open();
            DRV_ADC_Start();
            
            dbgOutputLoc(DBG_LOCATION_COMMTASK_START);
            unsigned int recv;
            unsigned char toggle = 0;
            
            writeUARTString("TEAM14", 6);
            dbgOutputLoc(DBG_LOCATION_COMMTASK_WHILE_LOOP);
            while (1)
            {
                dbgOutputLoc(DBG_LOCATION_COMMTASK_BEFORE_RECV);
                recv = recvFromQ();
                dbgOutputLoc(DBG_LOCATION_COMMTASK_AFTER_RECV);
                // send to debug fxns
                int cm = voltsToCm(recv);
                dbgUARTVal(cm);
                //dbgOutputVal(recv);
                
//                char buff[16];
//                sprintf(buff, "%f", recv);
//                writeUARTString(buff, 16);

            }
            
            break;
        }
        default:
        {
            break;
        }
    }
}
