#include "scanner.h"
#include "communication_globals.h"

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
    volts = -0.0000000705892998294409*volts*volts*volts + 0.000127490599545986*volts*volts - 0.076687973536683*volts + 17.8012614542364; // to in
    volts = 2.54*volts;
    return volts;
}

int abs(int x)
{
    if (x >= 0)
        return x;
    else 
        return -1*x;
}

void SCANNER_Initialize ( void )
{
    scannerData.state = SCANNER_STATE_INIT;
    scannerData.detectionFlag = true; // 1 means first object not detected
    scannerData.currZone = 1;
    scannerData.prevZone = 1;
    scannerData.red = 0;
    scannerData.green = 0;
    scannerData.blue = 0;
    scannerData.prevEdge = 0;
    scannerData.currXPos = 0;
    scannerData.signature = 0;
    scannerData.objectDetected = false;
    scannerData.colorFlag = false;
    DRV_ADC_Open();
    DRV_ADC_Start();
    DRV_TMR0_Start();
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
            struct scanQueueData rec;
            while(1) 
            {
                dbgOutputLoc(SCAN_THREAD_WAIT);
                if(xQueueReceive(scan_q, &rec, portMAX_DELAY))
                {
                    dbgOutputLoc(SCAN_THREAD_RECVD);
                    if(rec.type == COMM)
                    {
                        // Set current position
                        scannerData.currXPos = rec.xPos;
                        // Can detect new object, restart scanning
                        int widthThreshold = (int)(scannerData.currXPos - scannerData.prevEdge);
                        if (abs(widthThreshold) > OBJ_WIDTH) 
                        {
                            DRV_ADC_Open();
                            DRV_ADC_Start();
                            scannerData.objectDetected = false;
                        }
                        
                        // Set current zone
                        scannerData.currZone = rec.zone;
                        // Detected zone change so send object info
                        if (scannerData.currZone != scannerData.prevZone)
                        {
                            char buf[64];
                            sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, scannerData.prevZone, scannerData.red, scannerData.green, scannerData.blue, "FINISHED ZONE");
                            commSendMsgToUartQueue(buf);
                            // new zone, reset colors
                            scannerData.red = 0;
                            scannerData.green = 0;
                            scannerData.blue = 0;
                        }
                        scannerData.prevZone = scannerData.currZone; // update previous zone 
                        
                        if (rec.action == 0) // STOP scan from server
                        {
                            DRV_ADC_Stop();
                            DRV_TMR0_Stop();
                        }
                        else if (rec.action == 1) // START scan from server
                        {
                            DRV_ADC_Open();
                            DRV_ADC_Start();
                            DRV_TMR0_Start();
                        }
                        // else CONTINUE
                        break;
                    }
                    else if (rec.type == ADC)
                    {   
                        double cm = rec.dist;
                        // Objected detected, now determine color
                        if (cm <= OBJ_DISTANCE && (!scannerData.objectDetected || scannerData.detectionFlag)) // flag is for first object being detected
                        {    
                            scannerData.detectionFlag = false;
                            scannerData.prevEdge = scannerData.currXPos;
                            scannerData.objectDetected = true;
                            
                            // stop scanning
                            DRV_ADC_Stop();
                            // get color
                            scannerData.colorFlag = true;
                        }
                        break;
                    }
                    else if (rec.type == UART)
                    {
                        // if color hasn't been detected, try to get color
                        if (scannerData.colorFlag)
                        {
                            scannerData.signature = rec.signature;
                            // handle colors
                            if (scannerData.signature == 1)
                            {
                                scannerData.red += 1;
                            }
                            else if (scannerData.signature == 2)
                            {
                                scannerData.green += 1;
                            }
                            else if (scannerData.signature == 3)
                            {
                                scannerData.blue += 1;
                            }
                            scannerData.colorFlag = false;
                        }
                        break;
                    }
                    else if (rec.type == TMR)
                    {
                        // Sends info while scanning to retrieve position from server
                        char buf[64];
                        sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, scannerData.currZone, scannerData.red, scannerData.green, scannerData.blue, "SCANNING ZONE");
                        commSendMsgToUartQueue(buf);
                        break;
                    }
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
