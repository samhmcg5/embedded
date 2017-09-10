#include "communication.h"

COMMUNICATION_DATA communicationData;

// add a msg to the q
int sendMsgToQFromISR(unsigned char msg)
{
    xQueueSendFromISR(q, (void*)&msg, NULL);
    return 0;
}

// recv a message from the q
unsigned char recvFromQ()
{
    unsigned char recv;
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
            bool appInitialized = true;
            // create the q
            q = xQueueCreate(10, sizeof(unsigned char) );
            if (q == 0)
            {
                // failed to create
                // HALT
            }
        
            sendMsgToQFromISR('T');
            sendMsgToQFromISR('e');
            sendMsgToQFromISR('a');
            sendMsgToQFromISR('m');
            if (appInitialized)
            {
                communicationData.state = COMMUNICATION_STATE_RUNNING;
            }
            break;
        }
        case COMMUNICATION_STATE_RUNNING:
        {
            dbgOutputLoc(DBG_LOCATION_COMMTASK_START);
            unsigned char recv;
            unsigned char toggle = 0;
            dbgOutputLoc(DBG_LOCATION_COMMTASK_WHILE_LOOP);
            while (1)
            {
                dbgOutputLoc(DBG_LOCATION_COMMTASK_BEFORE_RECV);
                recv = recvFromQ();
                dbgOutputLoc(DBG_LOCATION_COMMTASK_AFTER_RECV);
                // send to debug fxns
                dbgUARTVal(recv);

                
//                dbgOutputLoc(0x34);
//                dbgOutputLoc(0x44);

            }
            
            break;
        }
        default:
        {
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
