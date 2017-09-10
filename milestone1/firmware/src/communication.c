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
                // HALT 
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
            unsigned char recv;
            while (1)
            {
                recv = recvFromQ();
                // send to debug fxns
                dbgUARTVal(recv);
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
