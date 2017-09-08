#include "communication.h"

COMMUNICATION_DATA communicationData;

int sendMsgToQ(unsigned char msg)
{
    xQueueSend(q, &msg, portMAX_DELAY);
}

void COMMUNICATION_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    communicationData.state = COMMUNICATION_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void COMMUNICATION_Tasks ( void )

  Remarks:
    See prototype in communication.h.
 */

void COMMUNICATION_Tasks ( void )
{
    switch ( communicationData.state )
    {
        case COMMUNICATION_STATE_INIT:
        {
            bool appInitialized = true;
            // create the q
            q = xQueueCreate(10, sizeof(unsigned char) );
        
            if (appInitialized)
            {
                communicationData.state = COMMUNICATION_STATE_RUNNING;
            }
            break;
        }
        case COMMUNICATION_STATE_RUNNING:
        {
            while (1)
            {
                
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
