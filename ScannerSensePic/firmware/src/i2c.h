#ifndef _I2C_H
#define _I2C_H

#include "debug.h"
#include "communication_globals.h"
#include "driver/i2c/drv_i2c.h"
#include "scan_globals.h"

typedef enum
{
	I2C_WAIT_FOR_OPEN=0,
    I2C_SEND_SLAVE_ADDRESS,
    I2C_WAIT_FOR_SLAVE_ADDRESS,
	I2C_SEND_READ_COLOR_DATA,
	I2C_WAIT_FOR_TRANSFER_TO_COMPLETE,
	I2C_FINISH_READ_COLOR_DATA,
    I2C_SENT_MSG_TO_SCAN_QUEUE,
    SERVER_TESTING,
} PIXY_STATES;

// Slave Address
#define PIXY_SLAVE_ADDRESS              0x54
// Bytes of data
#define PIXY_NUM_OF_BYTES               14

PIXY_STATES DRV_PIXY_HandleColors(DRV_HANDLE i2c_handle, PIXY_STATES state);
DRV_I2C_BUFFER_HANDLE DRV_PIXY_GetBlocks(DRV_HANDLE i2c_handle, unsigned char data[14]);
DRV_I2C_BUFFER_EVENT DRV_PIXY_CheckTransferStatus(DRV_HANDLE i2c_handle, DRV_I2C_BUFFER_HANDLE buffer_handle);

#endif