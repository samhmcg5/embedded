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
            tmr_message.msg_state = TMR_MESSAGE_STATE_T;
            bool appInitialized = true;
            // create the q
            q = xQueueCreate(10, sizeof(unsigned char) );
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
                dbgOutputVal('~');

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
