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

double voltsToCm(double volts)
{
    volts = 1/volts;
    volts = 4118.6*volts + 0.5;
    return volts;
}

void SCANNER_Initialize ( void )
{
    scannerData.state = SCANNER_STATE_INIT;
    zone = 0;
    red = 0;
    green = 0;
    blue = 0;
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
                scannerData.state = SCANNER_STATE_SCANNING;
            }
            break;
        }

        case SCANNER_STATE_SCANNING:
        {   
            DRV_ADC_Open();
            DRV_TMR0_Start();
            struct scanQueueData rec;
            while(1) 
            {
                dbgOutputLoc(SCAN_THREAD_WAIT);
                if(xQueueReceive(scan_q, &rec, portMAX_DELAY))
                {
                    dbgOutputLoc(SCAN_THREAD_RECVD);

                    if(rec.type == INFO)
                    {
                        zone = rec.zone;
                        if (rec.action == 0) // STOP
                        {
                            DRV_ADC_Stop();
                            DRV_TMR0_Stop();
                        }
                        else if (rec.action == 1) // START
                        {
                            DRV_ADC_Open();
                            DRV_ADC_Start();
                            DRV_TMR0_Start();
                        }
                        // else CONTINUE
                    }
                    else if (rec.type == ADC)
                    {
                        red = 0;
                        green = 0;
                        blue = 0;
                        
                        if (rec.color == 0) {
                            red = 1;
                        }
                        else if (rec.color == 1) {
                            green = 1;
                        }
                        else if (rec.color == 2) {
                            blue = 1;    
                        }
                        
                        double volts = (double)rec.dist;
                        double cm = voltsToCm(volts);
                        red = cm;
                    }
                    else if (rec.type == TMR)
                    {
                        char buf[64];
                        sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, zone, red, green, blue);    
                        commSendMsgToUartQueue(buf);   
                    }
                }
            }
        }

        default:
        {
            break;
        }
    }
}
