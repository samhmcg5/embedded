#include "scanner.h"
#include "communication_globals.h"

SCANNER_DATA scannerData;
unsigned int flag;

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
    prevzone = 1;
    zone = 1;
    red = 0;
    green = 0;
    blue = 0;
    prevEdge = 0;
    currXPos = 0;
    flag = 1;
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
                scannerData.i2c_handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
                //scannerData.state = SCANNER_STATE_SCANNING;
                scannerData.state = SCANNER_READ_WRITE;
            }
            break;
        }

        case SCANNER_STATE_SCANNING:
        {   
            // DRV_ADC_Open();
            // DRV_TMR0_Start();
            struct scanQueueData rec;
            while(1) 
            {
                dbgOutputLoc(SCAN_THREAD_WAIT);
                if(xQueueReceive(scan_q, &rec, portMAX_DELAY))
                {
                    dbgOutputLoc(SCAN_THREAD_RECVD);
                    if(rec.type == INFO)
                    {
                        // set current position and zone
                        currXPos = rec.x;
                        if (currXPos - prevEdge > 2)
                        {
                            DRV_ADC_Open();
                            DRV_ADC_Start();
                        }
                        zone = rec.zone;
                        if (zone != prevzone)
                        {
                            // Detected zone change so send object info
                            char buf[64];
                            sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, prevzone, red, green, blue, "FINISHED ZONE");    
                            commSendMsgToUartQueue(buf);
                            // new zone, reset colors
                            red = 0;
                            green = 0;
                            blue = 0;
                        }
                        prevzone = zone;
                        
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
                        double volts = (double)rec.dist;
                        double cm = voltsToCm(volts);
                        // objected detected, now determine color
                        if (cm <= 8.0 && (currXPos - prevEdge > 2 || flag == 1)) 
                        {
                            flag = 0;
                            scannerData.state = SCANNER_READ_WRITE;
                            break;
                        }
                    }
                    else if (rec.type == I2C)
                    {
                        // count colors 
                        char buf[64];
                        sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, rec.sync, rec.checksum, rec.signature, rec.xPos, "SCANNING ZONE");
                        commSendMsgToUartQueue(buf);
                        sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, rec.yPos, rec.width, rec.height, 69, "SCANNING ZONE");
                        commSendMsgToUartQueue(buf);
                    }
                    else if (rec.type == TMR)
                    {
                        // merely for debug info
                        char buf[64];
                        sprintf(buf, STR_ZONE_INFO, outgoing_seq, zone, red, green, blue, "SCANNING ZONE");    
                        commSendMsgToUartQueue(buf);   
                    }
                }
            }
            break;
        }
        // Read color data over I2C
        case SCANNER_READ_WRITE:
        {
            // FAKE SENSOR DATA FOR TESTING
            /*prevEdge = currXPos;
            if (zone == 1) {
                red += 1;
            }
            else if (zone == 2){
                green += 1;
            }
            else if (zone == 3){
                blue += 1;
            }
            DRV_ADC_Stop();
            */
            // COLOR SENSING
            DRV_PIXY_HandleColors(scannerData.i2c_handle);
            // scannerData.state = SCANNER_STATE_SCANNING;
            break;
        }
        default:
        {
            break;
        }
    }
}
