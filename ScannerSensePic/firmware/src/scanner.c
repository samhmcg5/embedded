#include "scanner.h"
#include "communication_globals.h"

SCANNER_DATA scannerData;

void sendMsgToScanQ(struct scanQueueData msg)
{
    xQueueSendToBack(scan_q, &msg, portMAX_DELAY);
}

void sendMsgToScanQFromISR(struct scanQueueData msg)
{
    xQueueSendToBackFromISR(scan_q, &msg, NULL);
}

void handleIncomingMsg(struct scanQueueData data)
{
    switch (data.type)
    {
        case INFO:
        {
            zone = data.zone;
            if (data.action == 0)
            {
                DRV_ADC_Stop();
                DRV_TMR0_Stop();
            }
            else
            {
                DRV_ADC_Open();
                DRV_ADC_Start();
                DRV_TMR0_Start();
            }
            break;
        }
        case ADC:
        {
            red = 0;
            green = 0;
            blue = 0;
            
            if (data.color == 0)
                red = 1;
            else if (data.color == 1)
                green = 1;
            else if (data.color == 2)
                blue = 1;
        }
        case TMR:
        {
            char buf[64];
            sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, zone, red, green, blue);    
            commSendMsgToUartQueue(buf);
            break;
        }
        default:
        {
            break;
        }
    }
}

void SCANNER_Initialize ( void )
{
    scannerData.state = SCANNER_STATE_INIT;
    scannerData.status = idle;
    scan_q = xQueueCreate(32, sizeof (struct scanQueueData));
}

void SCANNER_Tasks ( void )
{
    switch ( scannerData.state )
    {
        case SCANNER_STATE_INIT:
        {
            bool appInitialized = true;
            if (appInitialized)
            {
                zone = 0;
                red = 0;
                green = 0;
                blue = 0;
                scannerData.state = SCANNER_STATE_SCANNING;
                DRV_TMR0_Start();
            }
            break;
        }

        case SCANNER_STATE_SCANNING:
        {
            dbgOutputLoc(SCAN_THREAD_WAIT);    
          
            struct scanQueueData rec;
            if(xQueueReceive(scan_q, &rec, portMAX_DELAY))
            {
                dbgOutputLoc(SCAN_THREAD_RECVD);
                handleIncomingMsg(rec);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
