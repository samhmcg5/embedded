#ifndef _LINE_SENSOR_H
#define _LINE_SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "driver/i2c/drv_i2c.h"
#include "queue.h"

// Information specific to the TCS34725 RGB Color Sensor
// I2C address
#define TCS_I2C_ADDRESS 0x29
// Register locations
#define TCS_ENABLE_REGISTER 0x00
#define TCS_RGBC_TIMING_REGISTER 0x01
#define TCS_WAIT_TIMING_REGISTER 0x03
#define TCS_LOW_THRESHOLD_LOW_REGISTER 0x04
#define TCS_LOW_THRESHOLD_HIGH_REGISTER 0x05
#define TCS_HIGH_THRESHOLD_LOW_REGISTER 0x06
#define TCS_HIGH_THRESHOLD_HIGH_REGISTER 0x07
#define TCS_PERSISTENCE_REGISTER 0x0c
#define TCS_CONFIG_REGISTER 0x0d
#define TCS_CONTROL_REGISTER 0x0f
#define TCS_ID_REGISTER 0x12
#define TCS_STATUS_REGISTER 0x13
#define TCS_RGBC_CLEAR_LOW_REGISTER 0x14
#define TCS_RGBC_CLEAR_HIGH_REGISTER 0x15
#define TCS_RGBC_RED_LOW_REGISTER 0x16
#define TCS_RGBC_RED_HIGH_REGISTER 0x17
#define TCS_RGBC_GREEN_LOW_REGISTER 0x18
#define TCS_RGBC_GREEN_HIGH_REGISTER 0x19
#define TCS_RGBC_BLUE_LOW_REGISTER 0x1a
#define TCS_RGBC_BLUE_HIGH_REGISTER 0x1b
// Bit masks for registers
#define TCS_COMMAND_SELECT_MASK 0x80
#define TCS_ENABLE_AIEN_MASK 0x10
#define TCS_ENABLE_WEN_MASK 0x08
#define TCS_ENABLE_AEN_MASK 0x02
#define TCS_ENABLE_PON_MASK 0x01
#define TCS_PERSISTENCE_APERS_MASK 0x0f
#define TCS_CONFIG_WLONG_MASK 0x02
#define TCS_CONTROL_AGAIN_MASK 0x03
#define TCS_STATUS_AINT_MASK 0x10
#define TCS_STATUS_AVALID_MASK 0x01
#define TCS_REGISTER_ADDRESS_MASK 0x1f
// Values for the registers
#define TCS_COMMAND_AUTO_INCREMENT 0x20
#define TCS_AGAIN_1X 0x00
#define TCS_AGAIN_4X 0x01
#define TCS_AGAIN_16X 0x02
#define TCS_AGAIN_60X 0x03
// TCS State Machine states
#define STATE_WAITING_FOR_OPEN 0
#define STATE_INIT_COLOR_SENSOR 1
#define STATE_WAITING_FOR_TRANSFER_COMPLETED 2
#define STATE_CONFIG_ADC_GAIN 3
#define STATE_SEND_READ_COLOR_DATA 4
#define STATE_FINISH_READ_COLOR_DATA 5
#define STATE_CONFIG_COMMAND_REGISTER 6
#define STATE_SEND_READ_COLOR_DATA_ADDRESS 7
#define STATE_CONFIG_WAIT_TIME 8
#define STATE_CONFIG_ATIME 9
// Defines for information sent to line sensor
#define COLOR_SENSOR_ID_1 1
#define COLOR_SENSOR_ID_2 2
#define COLOR_SENSOR_RESET_STATE_MACHINE 5
#define COLOR_DATA_ID 0xcc

// Enable for server DEBUG output
int COLOR_SENSOR_SERVER_TESTING = 0;

// Handle color sensor globals
int HandlerState[4];
int PreviousState[4];
#define COLOR_DATA_LENGTH 8
unsigned char ColorData1[COLOR_DATA_LENGTH];
unsigned char ColorData2[COLOR_DATA_LENGTH];
DRV_I2C_BUFFER_HANDLE BufferHandle[4];

typedef enum
{
	/* Application's state machine's initial state. */
	LINE_SENSOR_STATE_INIT=0,
	LINE_SENSOR_STATE_SERVICE_TASKS,

} LINE_SENSOR_STATES;

typedef struct
{
    /* The application's current state */
    LINE_SENSOR_STATES state;

    /* I2C Driver variables  */
    DRV_HANDLE                              handleI2C0;
    DRV_HANDLE                              handleI2C1;
    DRV_I2C_BUFFER_HANDLE                   I2CBufferHandle;
    DRV_I2C_BUFFER_EVENT                    I2CBufferEvent;

} LINE_SENSOR_DATA;

// Incoming motor queue
QueueHandle_t sensor_q;

struct sensorQueueData
{
    char x; // FIXME - Garbage value used to drive line sensor speeds
};

void sendMsgToSensorQFromISR(struct sensorQueueData msg);
void sensorHandleIncomingMsg(struct sensorQueueData data);

// Motor action generating from line sensor values functions 
void generateMotorActions();
int leftOnTape();
int rightOnTape();

void LINE_SENSOR_Initialize ( void );
void LINE_SENSOR_Tasks( void );

// Harmony Driver TCS Color sensor drivers
//Initializes the color sensor
void DRV_TCS_Init(DRV_HANDLE i2c_handle, unsigned char waitTime, unsigned char gain);

//Configures the interrupt stuff for the color sensor
void DRV_TCS_IntConfig(DRV_HANDLE i2c_handle, unsigned short lowThreshold, unsigned short highThreshold, unsigned char persistence);

//Send a byte to the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_SendByte(DRV_HANDLE i2c_handle, unsigned char address, unsigned char data);

//Send an address to the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_SendAddress(DRV_HANDLE i2c_handle, unsigned char address);

//Receive a byte from the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_GetByte(DRV_HANDLE i2c_handle, unsigned char address, unsigned char *data);

//Read all of the color data from the color sensors
DRV_I2C_BUFFER_HANDLE DRV_TCS_GetColors(DRV_HANDLE i2c_handle, unsigned char data[8], unsigned char address);

//Handle all communication with the color sensor, generates motor control functions
int DRV_TCS_HandleColorSensor(DRV_HANDLE i2c_handle, int ColorSensorNumber);

#endif /* _LINE_SENSOR_H */