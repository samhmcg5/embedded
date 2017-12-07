#include "pixy.h"
#include "communication_globals.h"
#include "scan_globals.h"

uint8_t getByte()
{
    return (uint8_t) PLIB_USART_ReceiverByteReceive(USART_ID_2);
}

void getBlock()
{
    uint8_t data = getByte();
    uint16_t sync = (data << 8) | prevData;
    
    // detect if sync word has been sent
    if (sync == PIXY_START_WORD || sync == PIXY_START_WORDX)
    { 
        pixyBufferIdx = 0;
        pixyBuffer[pixyBufferIdx] = data;
        pixyBufferIdx++;
        pixyBuffer[pixyBufferIdx] = prevData; 
        syncFlag = true;
    }
    else if (syncFlag) { // sync word has been sent so rest is data
        pixyBufferIdx++;
        pixyBuffer[pixyBufferIdx] = data;
        if (pixyBufferIdx == PIXY_BLOCKSIZE - 1)
        {
            syncFlag = false;
            struct scanQueueData data;
            data.type = UART;
            data.sync = (pixyBuffer[0] << 8) | pixyBuffer[1];
            data.checksum = (pixyBuffer[3] << 8) | pixyBuffer[2];
            data.signature = (pixyBuffer[5] << 8) | pixyBuffer[4];
            data.objX = (pixyBuffer[7] << 8) | pixyBuffer[6];
            data.objY = (pixyBuffer[9] << 8) | pixyBuffer[8];
            data.objWidth = (pixyBuffer[11] << 8) | pixyBuffer[10];
            data.objHeight = (pixyBuffer[13] << 8) | pixyBuffer[12];
            sendMsgToScanQFromISR(data);
        }
    }
    prevData = data;
}