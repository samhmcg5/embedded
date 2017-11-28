#include "i2c.h"

DRV_I2C_BUFFER_HANDLE buffer_handle;
PIXY_STATES state = I2C_WAIT_FOR_OPEN;

// Check status of I2C transmission
DRV_I2C_BUFFER_EVENT DRV_PIXY_CheckTransferStatus(DRV_HANDLE i2c_handle, DRV_I2C_BUFFER_HANDLE buffer_handle)
{
    return (DRV_I2C_TransferStatusGet(i2c_handle, buffer_handle));
}

// Return block data of Pixy Cam 
DRV_I2C_BUFFER_HANDLE DRV_PIXY_GetBlocks(DRV_HANDLE i2c_handle, unsigned char data[14])
{
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_TransmitThenReceive(i2c_handle, PIXY_SLAVE_ADDRESS << 1, PIXY_SLAVE_ADDRESS << 1, 1, data, PIXY_NUM_OF_BYTES, NULL);
    return buffer_handle;
}

// Handles communication with the Pixy Cam
// Returns the color information of the detected object
int DRV_PIXY_HandleColors(DRV_HANDLE i2c_handle)
{
    unsigned char Block[PIXY_NUM_OF_BYTES];

    switch(state){
        case I2C_WAIT_FOR_OPEN:
        {
            if (i2c_handle == DRV_HANDLE_INVALID)
            {
                // error 
            }
            else
            {
                state =  I2C_SEND_READ_COLOR_DATA; 
            }
            break;
        }
        case I2C_SEND_READ_COLOR_DATA:
        {
            Nop();
            DRV_I2C_BUFFER_EVENT status;
            if (buffer_handle != NULL)
            {
                status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);
            }
            // Wait for bus to be free and get color data
            if(buffer_handle == NULL || status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                buffer_handle = DRV_PIXY_GetBlocks(i2c_handle, Block);
                state = I2C_WAIT_FOR_TRANSFER_TO_COMPLETE;
            }
            break;
        }
        case I2C_WAIT_FOR_TRANSFER_TO_COMPLETE:
        {
            Nop();
            DRV_I2C_BUFFER_EVENT status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);   
            if(status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                // Continue to next state if transfer is complete
                state = I2C_FINISH_READ_COLOR_DATA;
            }
            break;
        }
        case I2C_FINISH_READ_COLOR_DATA:
        {
            DRV_I2C_BUFFER_EVENT status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);
            if(status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                unsigned int sync = Block[0] | (Block[1] << 8);
                unsigned int checksum = Block[2] | (Block[3] << 8);
                unsigned int signature = Block[4] | (Block[5] << 8);
                unsigned int x = Block[6] | (Block[7] << 8);
                unsigned int y = Block[8] | (Block[9] << 8);
                unsigned int width = Block[10] | (Block[11] << 8);
                unsigned int height = Block[12] | (Block[13] << 8);
                
                char buf[64];
                sprintf(buf, STR_UPDATE_ZONE_QUOTAS, sync, checksum, signature, x, 70, "SCANNING ZONE");
                commSendMsgToUartQueue(buf);
                sprintf(buf, STR_UPDATE_ZONE_QUOTAS, y, width, height, x, 70, "SCANNING ZONE");
                commSendMsgToUartQueue(buf);
                
                /*struct scanQueueData msg;
                msg.type == I2C;
                msg.sync = sync;
                msg.checksum = checksum;
                msg.signature = signature;
                msg.xPos = x;
                msg.yPos = y;
                msg.width = width;
                msg.height = height;*/
                //sendMsgToScanQ(msg);
                state = I2C_WAIT_FOR_OPEN;
                return 1;
            }
            break;
        }
        return -1;
    }
}