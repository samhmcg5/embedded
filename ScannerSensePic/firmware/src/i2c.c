#include "i2c.h"

DRV_I2C_BUFFER_HANDLE buffer_handle;
uint8_t TXbuffer[] = "\x54";

// Check status of I2C transmission
DRV_I2C_BUFFER_EVENT DRV_PIXY_CheckTransferStatus(DRV_HANDLE i2c_handle, DRV_I2C_BUFFER_HANDLE buffer_handle)
{
    return (DRV_I2C_TransferStatusGet(i2c_handle, buffer_handle));
}

// Return block data of Pixy Cam 
DRV_I2C_BUFFER_HANDLE DRV_PIXY_GetBlocks(DRV_HANDLE i2c_handle, unsigned char data[6])
{
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_TransmitThenReceive(i2c_handle, PIXY_SLAVE_ADDRESS, &TXbuffer, 1, data, PIXY_NUM_OF_BYTES, NULL);
    return buffer_handle;
}

// Send slave address to Pixy Cam
DRV_I2C_BUFFER_HANDLE DRV_PIXY_SendAddress(DRV_HANDLE i2c_handle)
{
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_Transmit(i2c_handle, PIXY_SLAVE_ADDRESS, &TXbuffer, 1, NULL);
    return buffer_handle;
}
unsigned int error = 0;

// Handles communication with the Pixy Cam
// Returns the color information of the detected object
PIXY_STATES DRV_PIXY_HandleColors(DRV_HANDLE i2c_handle, PIXY_STATES state)
{
    unsigned char Block[PIXY_NUM_OF_BYTES];
    
    switch(state){
        case SERVER_TESTING:
        {
            char buf[64];
            sprintf(buf, STR_I2C_ERROR, outgoing_seq, error);    
            commSendMsgToUartQueue(buf);
            return SERVER_TESTING;
            break;
        }
        case I2C_WAIT_FOR_OPEN:
        {
            if (i2c_handle == DRV_HANDLE_INVALID)
            {
                // Error occurred while opening I2C Module
                //error = "I2C Module did not open correctly!";
                error = 1;
                return I2C_WAIT_FOR_OPEN;
            }
            else
            {
                return I2C_SEND_READ_COLOR_DATA; 
            }
            break;
        }
        case I2C_SEND_SLAVE_ADDRESS:
        {
            DRV_I2C_BUFFER_EVENT status;
            if (buffer_handle != NULL)
            {
                status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);
            }
            if (buffer_handle == NULL || status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                buffer_handle = DRV_PIXY_SendAddress(i2c_handle);
                return I2C_WAIT_FOR_SLAVE_ADDRESS;
            }
            return I2C_SEND_SLAVE_ADDRESS;
            break;
        }
        case I2C_WAIT_FOR_SLAVE_ADDRESS:
        {
            DRV_I2C_BUFFER_EVENT status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);   
            
            /*    
            if (status == DRV_I2C_BUFFER_MASTER_ACK_SEND)
            {
                Nop();
            }
            if (status == DRV_I2C_SEND_STOP_EVENT)
            {
                Nop();
            }
            if (status == DRV_I2C_SEND_RESTART_EVENT)
            {
                Nop();
            }*/
            if (status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                // slave address sent, now ready to read data
                return I2C_SEND_READ_COLOR_DATA;
            }
            else if (status == DRV_I2C_BUFFER_SLAVE_WRITE_REQUESTED) {
                error = 2;
                return SERVER_TESTING;
            }
            else if (status == DRV_I2C_BUFFER_SLAVE_READ_REQUESTED) {
                error = 3;
                return SERVER_TESTING;
            }
            return I2C_WAIT_FOR_SLAVE_ADDRESS;
            break;
        }
        case I2C_SEND_READ_COLOR_DATA:
        {
            DRV_I2C_BUFFER_EVENT status;
            if (buffer_handle != NULL)
            {
                status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);
            }
            // Wait for bus to be free and get color data
            if(buffer_handle == NULL || status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                buffer_handle = DRV_PIXY_GetBlocks(i2c_handle, Block);
                return I2C_WAIT_FOR_TRANSFER_TO_COMPLETE;
            }
            return I2C_SEND_READ_COLOR_DATA;
            break;
        }
        case I2C_WAIT_FOR_TRANSFER_TO_COMPLETE:
        {
            DRV_I2C_BUFFER_EVENT status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);   
            /*if (status == DRV_I2C_BUFFER_SLAVE_READ_REQUESTED)
            {
                Nop();
            }
            if (status == DRV_I2C_BUFFER_SLAVE_WRITE_REQUESTED)
            {
                Nop();
            }    
            if (status == DRV_I2C_BUFFER_MASTER_ACK_SEND)
            {
                Nop();
            }
            if (status == DRV_I2C_SEND_STOP_EVENT)
            {
                Nop();
            }
            if (status == DRV_I2C_SEND_RESTART_EVENT)
            {
                Nop();
            }*/
            if(status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                // Continue to next state if transfer is complete
                return I2C_FINISH_READ_COLOR_DATA;
            }
            return I2C_WAIT_FOR_TRANSFER_TO_COMPLETE;
            break;
        }
        case I2C_FINISH_READ_COLOR_DATA:
        {
            DRV_I2C_BUFFER_EVENT status = DRV_PIXY_CheckTransferStatus(i2c_handle, buffer_handle);
            if(buffer_handle == NULL || status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR)
            {
                unsigned int sync;
                sync = Block[0] | (Block[1] << 8);
                struct scanQueueData msg;
                msg.type = I2C;
                //msg.color = sync;
                char buf[64];
                sprintf(buf, STR_UPDATE_ZONE_QUOTAS, outgoing_seq, 0, 0, 0, sync, "COLOR TESTING");    
                commSendMsgToUartQueue(buf);
                return I2C_SENT_MSG_TO_SCAN_QUEUE;
            }
            /*else
            {
                // error = "Reading data from block did not work!";
                error = 4;
                return SERVER_TESTING;
            }*/
            return I2C_FINISH_READ_COLOR_DATA;
            break;
        }
        case I2C_SENT_MSG_TO_SCAN_QUEUE:
        {
            // idle state waiting for read again
            return I2C_SENT_MSG_TO_SCAN_QUEUE;
            break;
        }
    }
}