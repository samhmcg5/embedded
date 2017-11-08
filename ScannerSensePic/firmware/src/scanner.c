#include "scanner.h"
#include "communication_globals.h"

#define PIXY_SLAVE_ADDRESS              0x54
#define PIXY_NUM_OF_BYTES               14

SCANNER_DATA scannerData;

DRV_I2C_BUFFER_EVENT i2cOpStatus;

uint8_t operationStatus;

uint8_t deviceAddressSlave;

uint8_t indexRegByteSize;

uint8_t         TXbuffer[] = "\x54";

uint16_t        RXbuffer[100];

typedef enum{
    
        TxRx_TO_PIXY = 0,
        TxRx_COMPLETED

}I2C_STATES;

static I2C_STATES readWriteState = TxRx_TO_PIXY;

DRV_I2C_BUFFER_EVENT SCANNER_Check_Transfer_Status(DRV_HANDLE drvOpenHandle, DRV_I2C_BUFFER_HANDLE drvBufferHandle);

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
                scannerData.drvI2CHandle_Master = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
                scannerData.state = SCANNER_STATE_SCANNING;

                if(scannerData.drvI2CHandle_Master != (DRV_HANDLE) NULL)
                {
                    scannerData.state = SCANNER_READ_WRITE;
                }
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
        
        case SCANNER_READ_WRITE:
        {
            if(SCANNER_Read_Tasks())
            {
                scannerData.state = SCANNER_STATE_SCANNING;
            }
        }
        default:
        {
            break;
        }
    }
}

bool SCANNER_Read_Tasks(void)
{
    switch (readWriteState)
    {
        case TxRx_TO_PIXY:
        {    

            deviceAddressSlave = PIXY_SLAVE_ADDRESS;

            /* Write Read Transaction to PixyCAM */

            if ( (scannerData.drvI2CTxRxBufferHandle[0] == (DRV_I2C_BUFFER_HANDLE) NULL) || 
                    (SCANNER_Check_Transfer_Status(scannerData.drvI2CHandle_Master, scannerData.drvI2CTxRxBufferHandle[0]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || 
                        (SCANNER_Check_Transfer_Status(scannerData.drvI2CHandle_Master, scannerData.drvI2CTxRxBufferHandle[0]) == DRV_I2C_BUFFER_EVENT_ERROR) )
            {
                scannerData.drvI2CTxRxBufferHandle[0] = DRV_I2C_TransmitThenReceive (scannerData.drvI2CHandle_Master,
                                                                                    deviceAddressSlave,
                                                                                    &TXbuffer[0], 
                                                                                    (sizeof(TXbuffer)-1),
                                                                                    &RXbuffer[0],
                                                                                    PIXY_NUM_OF_BYTES,
                                                                                    NULL);
                char buf[64];
                sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, RXbuffer[0], RXbuffer[1], RXbuffer[2], RXbuffer[3]);    
                commSendMsgToUartQueue(buf);
            }
            break;
        }   
        case TxRx_COMPLETED:
        {
            return true;
            break;
        }
    }
    return false;

}

DRV_I2C_BUFFER_EVENT SCANNER_Check_Transfer_Status(DRV_HANDLE drvOpenHandle, DRV_I2C_BUFFER_HANDLE drvBufferHandle)
{
    return (DRV_I2C_TransferStatusGet  (scannerData.drvI2CHandle_Master, drvBufferHandle));
}
