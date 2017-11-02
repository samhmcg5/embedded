#include "i2c.h"
#include "driver/i2c/drv_i2c.h"

//------------------------------------------------------------------------------
//PLIB TCS Color sensor control functions
//------------------------------------------------------------------------------
//Initializes the color sensor
void TCS_Init(I2C_MODULE_ID bus, unsigned char waitTime, unsigned char gain){
    //Initialize the device
    unsigned char enableData = TCS_GetByte(bus, TCS_ENABLE_REGISTER);
    TCS_SendByte(bus, TCS_ENABLE_REGISTER, (enableData | (TCS_ENABLE_PON_MASK | TCS_ENABLE_AIEN_MASK | TCS_ENABLE_AEN_MASK | TCS_ENABLE_WEN_MASK)));
    //Set the wait time
    TCS_SendByte(bus, TCS_WAIT_TIMING_REGISTER, waitTime);
    //Set the ADC gain
    TCS_SendByte(bus, TCS_CONTROL_REGISTER, (gain & TCS_CONTROL_AGAIN_MASK));
}

//Configures the interrupt stuff for the color sensor
void TCS_IntConfig(I2C_MODULE_ID bus, unsigned short lowThreshold, unsigned short highThreshold, unsigned char persistence){
    //Set the interrupt thresholds
    TCS_SendByte(bus, TCS_LOW_THRESHOLD_LOW_REGISTER, (lowThreshold & 0x00ff));
    TCS_SendByte(bus, TCS_LOW_THRESHOLD_HIGH_REGISTER, ((lowThreshold & 0xff00) >> 8));
    TCS_SendByte(bus, TCS_HIGH_THRESHOLD_LOW_REGISTER, (highThreshold & 0x00ff));
    TCS_SendByte(bus, TCS_HIGH_THRESHOLD_HIGH_REGISTER, ((highThreshold & 0xff00) >> 8));
    //Set the persistence value
    TCS_SendByte(bus, TCS_PERSISTENCE_REGISTER, (persistence & TCS_PERSISTENCE_APERS_MASK));
}

//Send a byte to the color sensor
void TCS_SendByte(I2C_MODULE_ID bus, unsigned char address, unsigned char data){
    MyI2CStartBus(bus);
    MyI2CSendByte(bus, CreateAddressWord(TCS_I2C_ADDRESS, I2C_WRITE));
    MyI2CSendByte(bus, address);
    MyI2CSendByte(bus, data);
    MyI2CStopBus(bus);
}

//Receive a byte from the color sensor
unsigned char TCS_GetByte(I2C_MODULE_ID bus, unsigned char address){
    MyI2CStartBus(bus);
    MyI2CSendByte(bus, CreateAddressWord(TCS_I2C_ADDRESS, I2C_WRITE));
    MyI2CSendByte(bus, address);
    MyI2CRepeatStartBus(bus);
    MyI2CSendByte(bus, CreateAddressWord(TCS_I2C_ADDRESS, I2C_READ));
    unsigned char data = MyI2CReadByte(bus);
    MyI2CStopBus(bus);
    return data;
}

//------------------------------------------------------------------------------
//Harmony Driver TCS Color sensor control functions
//------------------------------------------------------------------------------
//Initializes the color sensor
void DRV_TCS_Init(DRV_HANDLE i2c_handle, unsigned char waitTime, unsigned char gain){
    //Initialize the device
    unsigned char enableData;
    DRV_TCS_GetByte(i2c_handle, TCS_ENABLE_REGISTER, &enableData);
    DRV_TCS_SendByte(i2c_handle, TCS_ENABLE_REGISTER, (enableData | (TCS_ENABLE_PON_MASK | TCS_ENABLE_AEN_MASK)));
    //DRV_TCS_SendByte(i2c_handle, TCS_ENABLE_REGISTER, (TCS_ENABLE_PON_MASK | TCS_ENABLE_AEN_MASK));
    //Set the wait time
    DRV_TCS_SendByte(i2c_handle, TCS_WAIT_TIMING_REGISTER, waitTime);
    //Set the ADC gain
    DRV_TCS_SendByte(i2c_handle, TCS_CONTROL_REGISTER, (gain & TCS_CONTROL_AGAIN_MASK));
}

//Configures the interrupt stuff for the color sensor
void DRV_TCS_IntConfig(DRV_HANDLE i2c_handle, unsigned short lowThreshold, unsigned short highThreshold, unsigned char persistence){
    //Set the interrupt thresholds
    DRV_TCS_SendByte(i2c_handle, TCS_LOW_THRESHOLD_LOW_REGISTER, (lowThreshold & 0x00ff));
    DRV_TCS_SendByte(i2c_handle, TCS_LOW_THRESHOLD_HIGH_REGISTER, ((lowThreshold & 0xff00) >> 8));
    DRV_TCS_SendByte(i2c_handle, TCS_HIGH_THRESHOLD_LOW_REGISTER, (highThreshold & 0x00ff));
    DRV_TCS_SendByte(i2c_handle, TCS_HIGH_THRESHOLD_HIGH_REGISTER, ((highThreshold & 0xff00) >> 8));
    //Set the persistence value
    DRV_TCS_SendByte(i2c_handle, TCS_PERSISTENCE_REGISTER, (persistence & TCS_PERSISTENCE_APERS_MASK));
}

//Send a byte to the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_SendByte(DRV_HANDLE i2c_handle, unsigned char address, unsigned char data){
    unsigned char send_buffer[2];
    send_buffer[0] = (address & TCS_REGISTER_ADDRESS_MASK) | (TCS_COMMAND_SELECT_MASK);
    send_buffer[1] = data;
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_Transmit(i2c_handle, TCS_I2C_ADDRESS << 1, send_buffer, 2, NULL);
    return buffer_handle;
}

//Send an address to the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_SendAddress(DRV_HANDLE i2c_handle, unsigned char address){
    address &= TCS_REGISTER_ADDRESS_MASK;
    address |= (TCS_COMMAND_SELECT_MASK);
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_Transmit(i2c_handle, TCS_I2C_ADDRESS << 1, &address, 1, NULL);
    return buffer_handle;
}

//Receive a byte from the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_GetByte(DRV_HANDLE i2c_handle, unsigned char address, unsigned char *data){
    address &= TCS_REGISTER_ADDRESS_MASK;
    address |= (TCS_COMMAND_SELECT_MASK);
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_TransmitThenReceive (i2c_handle, TCS_I2C_ADDRESS << 1, &address, 1, data, 1, NULL);
    return buffer_handle;
}

//Read all of the color data from the color sensors
//Returns the colors in this order: (msB) (BH, BL, GH, GL, RH, RL, CH, CL) (msB)
DRV_I2C_BUFFER_HANDLE DRV_TCS_GetColors(DRV_HANDLE i2c_handle, unsigned char data[8], unsigned char address){
    //unsigned char address = TCS_RGBC_CLEAR_LOW_REGISTER;
    address &= TCS_REGISTER_ADDRESS_MASK;
    address |= (TCS_COMMAND_SELECT_MASK);
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_TransmitThenReceive (i2c_handle, TCS_I2C_ADDRESS << 1, &address, 1, data, 8, NULL);
    return buffer_handle;
}

//Handle all communication with the color sensor
//Sends color data to the NAV queue when the data is ready
//Returns the current state of the communication
int HandlerState[4];
int PreviousState[4];
unsigned char OnBlueTape[4];
#define COLOR_DATA_LENGTH 8
unsigned char ColorData1[COLOR_DATA_LENGTH];
unsigned char ColorData2[COLOR_DATA_LENGTH];
unsigned char ColorData3[COLOR_DATA_LENGTH];
unsigned char ColorData4[COLOR_DATA_LENGTH];
DRV_I2C_BUFFER_HANDLE BufferHandle[4];

//unsigned char testServerMsg[SEND_QUEUE_BUFFER_SIZE];
int DRV_TCS_HandleColorSensor(DRV_HANDLE i2c_handle, int ColorSensorNumber){
    //FOR TESTING
        if (COLOR_SENSOR_SERVER_TESTING){
            unsigned char testServerMsg[256];
            sprintf(testServerMsg, "*State Machine Enter, Sensor %d, State %d~", ColorSensorNumber, HandlerState[ColorSensorNumber-1]);
            commSendMsgToUartQueue(testServerMsg);
        }
    //END FOR TESTING
    if (ColorSensorNumber == COLOR_SENSOR_RESET_STATE_MACHINE){
        //Reset the state machine
        HandlerState[0] = STATE_WAITING_FOR_OPEN;
        HandlerState[1] = STATE_WAITING_FOR_OPEN;
        HandlerState[2] = STATE_WAITING_FOR_OPEN;
        HandlerState[3] = STATE_WAITING_FOR_OPEN;
        PreviousState[0] = STATE_WAITING_FOR_OPEN;
        PreviousState[1] = STATE_WAITING_FOR_OPEN;
        PreviousState[2] = STATE_WAITING_FOR_OPEN;
        PreviousState[3] = STATE_WAITING_FOR_OPEN;
        BufferHandle[0] = NULL;
        BufferHandle[1] = NULL;
        BufferHandle[2] = NULL;
        BufferHandle[3] = NULL;
        Nop();
        return 0;
    }
    switch (HandlerState[ColorSensorNumber-1]){
        case STATE_WAITING_FOR_OPEN:{
            //Wait for the i2c to be opened, and handle any errors that happen
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Waiting For Open~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if (i2c_handle == DRV_HANDLE_INVALID){
                //Error occurred while opening the I2C module
                Nop();
            }else{
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_CONFIG_COMMAND_REGISTER;
                Nop();
            }
            break;
        }
        case STATE_WAITING_FOR_TRANSFER_COMPLETED:{
            //Wait for the transfer to finish
            DRV_I2C_BUFFER_EVENT status = DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]);
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Waiting For Transfer Complete: %d~", status);
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if (status == DRV_I2C_BUFFER_EVENT_COMPLETE || status == DRV_I2C_BUFFER_EVENT_ERROR){
                //Move on if the transfer is complete
                switch (PreviousState[ColorSensorNumber-1]){
                    case STATE_INIT_COLOR_SENSOR:{
                        //Move to setting the wait time
                        PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                        HandlerState[ColorSensorNumber-1] = STATE_CONFIG_WAIT_TIME;
                        break;
                    }
                    case STATE_CONFIG_COMMAND_REGISTER:{
                        //Move to initializing the color sensor
                        PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                        HandlerState[ColorSensorNumber-1] = STATE_INIT_COLOR_SENSOR;
                        break;
                    }
                    case STATE_CONFIG_WAIT_TIME:{
                        //Move to setting the color sensor's ADC timing
                        PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                        HandlerState[ColorSensorNumber-1] = STATE_CONFIG_ATIME;
                        break;
                    }
                    case STATE_CONFIG_ATIME:{
                        //Move to setting the color sensor's ADC gain
                        PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                        HandlerState[ColorSensorNumber-1] = STATE_CONFIG_ADC_GAIN;
                        break;
                    }
                    case STATE_CONFIG_ADC_GAIN:{
                        //Move to send the address for the color read
                        PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
    //                        HandlerState = STATE_SEND_READ_COLOR_DATA_ADDRESS;
                        HandlerState[ColorSensorNumber-1] = STATE_SEND_READ_COLOR_DATA;
                        break;
                    }
                    case STATE_SEND_READ_COLOR_DATA_ADDRESS:{
                        //Move to begin reading color sensor data
                        PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                        HandlerState[ColorSensorNumber-1] = STATE_SEND_READ_COLOR_DATA;
                        break;
                    }
                    case STATE_SEND_READ_COLOR_DATA:{
                        //Move to finish reading color sensor data
                        PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                        HandlerState[ColorSensorNumber-1] = STATE_FINISH_READ_COLOR_DATA;
                        break;
                    }
                }
            }
            break;
        }
        case STATE_INIT_COLOR_SENSOR:{
            //Wait for the bus to be free, then initialize the color sensor
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Init Color Sensor~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
                BufferHandle[ColorSensorNumber-1] = DRV_TCS_SendByte(i2c_handle, TCS_ENABLE_REGISTER, (TCS_ENABLE_PON_MASK | TCS_ENABLE_AEN_MASK));// | TCS_ENABLE_WEN_MASK));
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_WAITING_FOR_TRANSFER_COMPLETED;
            }
            break;
        }
        case STATE_CONFIG_COMMAND_REGISTER:{
            //Wait for the bus to be free, then configure read protocol with the command register
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Config Command Register~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
    //                BufferHandle = DRV_TCS_SendByte(i2c_handle, (TCS_COMMAND_SELECT_MASK | TCS_ENABLE_REGISTER), (TCS_COMMAND_SELECT_MASK | TCS_COMMAND_AUTO_INCREMENT));
                unsigned char message[2];
                message[0] = (TCS_COMMAND_SELECT_MASK | TCS_ENABLE_REGISTER);
                message[1] = (TCS_COMMAND_AUTO_INCREMENT);
    //                message[1] = (TCS_COMMAND_SELECT_MASK | TCS_COMMAND_AUTO_INCREMENT);
                BufferHandle[ColorSensorNumber-1] = DRV_I2C_Transmit(i2c_handle, TCS_I2C_ADDRESS << 1, message, 2, NULL);
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_WAITING_FOR_TRANSFER_COMPLETED;
            }
            break;
        }
        case STATE_CONFIG_WAIT_TIME:{
            //Wait for the bus to be free, then set wait time
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Config Wait Time~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
                BufferHandle[ColorSensorNumber-1] = DRV_TCS_SendByte(i2c_handle, TCS_WAIT_TIMING_REGISTER, 0xff);
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_WAITING_FOR_TRANSFER_COMPLETED;
            }
            break;
        }
        case STATE_CONFIG_ATIME:{
            //Wait for the bus to be free, then set ADC time
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Config ADC Time~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
                BufferHandle[ColorSensorNumber-1] = DRV_TCS_SendByte(i2c_handle, TCS_RGBC_TIMING_REGISTER, 0xc0);
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_WAITING_FOR_TRANSFER_COMPLETED;
            }
            break;
        }
        case STATE_CONFIG_ADC_GAIN:{
            //Wait for the bus to be free, then set the ADC gain
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Config ADC Gain~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
                BufferHandle[ColorSensorNumber-1] = DRV_TCS_SendByte(i2c_handle, TCS_CONTROL_REGISTER, (TCS_AGAIN_1X & TCS_CONTROL_AGAIN_MASK));
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_WAITING_FOR_TRANSFER_COMPLETED;
            }
            break;
        }
        case STATE_SEND_READ_COLOR_DATA_ADDRESS:{
            //Wait for the bus to be free, then send the address to the color data
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Send Read Color Data Address~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
                BufferHandle[ColorSensorNumber-1] = DRV_TCS_SendAddress(i2c_handle, TCS_RGBC_CLEAR_LOW_REGISTER);
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_WAITING_FOR_TRANSFER_COMPLETED;
            }
            break;
        }
        case STATE_SEND_READ_COLOR_DATA:{
            //Wait for the bus to be free, then get the color data
            //FOR TESTING
            if (COLOR_SENSOR_SERVER_TESTING){
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "*State Send Read Color Data~");
                commSendMsgToUartQueue(testServerMsg);
            }
            //END FOR TESTING
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
                if (ColorSensorNumber == COLOR_SENSOR_ID_1){
                    BufferHandle[ColorSensorNumber-1] = DRV_TCS_GetColors(i2c_handle, ColorData1, TCS_RGBC_CLEAR_LOW_REGISTER);
                }
                else if (ColorSensorNumber == COLOR_SENSOR_ID_2){
                    BufferHandle[ColorSensorNumber-1] = DRV_TCS_GetColors(i2c_handle, ColorData2, TCS_RGBC_CLEAR_LOW_REGISTER);
                }
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_WAITING_FOR_TRANSFER_COMPLETED;
            }
            break;
        }
        case STATE_FINISH_READ_COLOR_DATA:{
            //Handle sending the color data to the NAV queue
            if ((BufferHandle[ColorSensorNumber-1] == NULL) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_COMPLETE) || (DRV_I2C_TransferStatusGet(i2c_handle, BufferHandle[ColorSensorNumber-1]) == DRV_I2C_BUFFER_EVENT_ERROR)){
                unsigned int c;
                unsigned int r;
                unsigned int g;
                unsigned int b;
                if (ColorSensorNumber == COLOR_SENSOR_ID_1){
                    c = ColorData1[0] | (ColorData1[1] << 8);
                    r = ColorData1[2] | (ColorData1[3] << 8);
                    g = ColorData1[4] | (ColorData1[5] << 8);
                    b = ColorData1[6] | (ColorData1[7] << 8);
                }
                else if (ColorSensorNumber == COLOR_SENSOR_ID_2){
                    c = ColorData2[0] | (ColorData2[1] << 8);
                    r = ColorData2[2] | (ColorData2[3] << 8);
                    g = ColorData2[4] | (ColorData2[5] << 8);
                    b = ColorData2[6] | (ColorData2[7] << 8);
                }
                //Send information to Navigation
                if (ColorSensorNumber == COLOR_SENSOR_ID_1){
                    //Front Left Color Sensor
                    // Send to UART
                    
                    
                }
                
                Nop();
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
    //                HandlerState = STATE_SEND_READ_COLOR_DATA_ADDRESS;
                HandlerState[ColorSensorNumber-1] = STATE_SEND_READ_COLOR_DATA;
            }
            break;
        }
    }
    return HandlerState[ColorSensorNumber-1];
}

//------------------------------------------------------------------------------
//I2C helper functions
//------------------------------------------------------------------------------
//Creates the 8-bit address from a 7-bit address and a read/write bit
unsigned char CreateAddressWord(unsigned char address, unsigned char readWrite){
    unsigned char word = (address << 1) | (readWrite & 0x01);
    return word;
}

//------------------------------------------------------------------------------
//PLIB I2C communication functions
//------------------------------------------------------------------------------
//Take control on an I2C bus and begin transmission
bool MyI2CStartBus(I2C_MODULE_ID bus){
    if (!PLIB_I2C_BusIsIdle(bus)){
        //Return if the bus is not idle
        return 0;
    }
    //Start the transmission
    PLIB_I2C_MasterStart(bus);
    
    return 1;
}

//Restart transmission on a bus that you already control
bool MyI2CRepeatStartBus(I2C_MODULE_ID bus){
    //Restart the bus
    PLIB_I2C_MasterStartRepeat(bus);
    
    return 1;
}

//Send the stop signal, then end the transmission
bool MyI2CStopBus(I2C_MODULE_ID bus){
    //Stop the bus
    PLIB_I2C_MasterStop(bus);
    
    return 1;
}

//Initialize the bus
bool MyI2CInit(I2C_MODULE_ID bus, unsigned int sourceFrequency, unsigned int baudRate){
    //Set the baud rate
    PLIB_I2C_BaudRateSet(bus, sourceFrequency, baudRate);
    //Enable the bus
    PLIB_I2C_Enable(bus);
    
    return 1;
}

//Send a byte of data over the bus
bool MyI2CSendByte(I2C_MODULE_ID bus, unsigned char data){
    if (!PLIB_I2C_TransmitterIsReady(bus)){
        //Return false if the transmitter is not ready
        return 0;
    }
    //Send the data
    PLIB_I2C_TransmitterByteSend(bus, data);
    if (!PLIB_I2C_TransmitterByteHasCompleted(bus)){
        //Wait for the transmit to complete
    }
    if (!PLIB_I2C_TransmitterByteWasAcknowledged(bus)){
        //Wait for an acknowledgement
    }
    
    return 1;
}

//Receive a byte of data from the bus
unsigned char MyI2CReadByte(I2C_MODULE_ID bus){
    //Clock a byte from the slave device
    PLIB_I2C_MasterReceiverClock1Byte(bus);
    if (!PLIB_I2C_ReceivedByteIsAvailable(bus)){
        //Return if there is no data available
        return 0;
    }
    //Send an acknowlegement
    PLIB_I2C_ReceivedByteAcknowledge(bus, 1);
    if (!PLIB_I2C_ReceiverByteAcknowledgeHasCompleted(bus)){
        //Wait for the ack to complete
    }
    //Get the byte from the receiver
    return PLIB_I2C_ReceivedByteGet(bus);
}

//------------------------------------------------------------------------------
//Server testing functions
//------------------------------------------------------------------------------
void ServerTestColorSensor(I2C_MODULE_ID bus){
    //    if (COLOR_SENSOR_SERVER_TESTING == 0){
    //        //Do not preform the test if not testing
    //        return;
    //    }
    unsigned char clow = TCS_GetByte(bus, TCS_RGBC_CLEAR_LOW_REGISTER);
    unsigned char chigh = TCS_GetByte(bus, TCS_RGBC_CLEAR_HIGH_REGISTER);
    unsigned char rlow = TCS_GetByte(bus, TCS_RGBC_RED_LOW_REGISTER);
    unsigned char rhigh = TCS_GetByte(bus, TCS_RGBC_RED_HIGH_REGISTER);
    unsigned char glow = TCS_GetByte(bus, TCS_RGBC_GREEN_LOW_REGISTER);
    unsigned char ghigh = TCS_GetByte(bus, TCS_RGBC_GREEN_HIGH_REGISTER);
    unsigned char blow = TCS_GetByte(bus, TCS_RGBC_BLUE_LOW_REGISTER);
    unsigned char bhigh = TCS_GetByte(bus, TCS_RGBC_BLUE_HIGH_REGISTER);
    unsigned int c = clow | (chigh << 8);
    unsigned int r = rlow | (rhigh << 8);
    unsigned int g = glow | (ghigh << 8);
    unsigned int b = blow | (bhigh << 8);
    unsigned char testServerMsg[256];
    unsigned char id = TCS_GetByte(bus, TCS_ID_REGISTER);
    sprintf(testServerMsg, "*{\"S\":\"s\",\"T\":\"v\",\"M\":\"s\",\"N\":0,\"D\":[%d,%d,%d,%d,%d],\"C\":123}~", id, c, r, g, b);
    commSendMsgToUartQueue(testServerMsg);
}


void ServerTestColorSensor_Driver(DRV_HANDLE i2c_handle){
    unsigned char colorData[8];
    DRV_TCS_GetColors(i2c_handle, colorData, TCS_RGBC_CLEAR_LOW_REGISTER);
    unsigned int c = colorData[0] | (colorData[1] << 8);
    unsigned int r = colorData[2] | (colorData[3] << 8);
    unsigned int g = colorData[4] | (colorData[5] << 8);
    unsigned int b = colorData[6] | (colorData[7] << 8);
    unsigned char testServerMsg[256];
    unsigned char id;
    DRV_TCS_GetByte(i2c_handle, TCS_ID_REGISTER, &id);
    sprintf(testServerMsg, "*{\"S\":\"s\",\"T\":\"v\",\"M\":\"s\",\"N\":0,\"D\":[%d,%d,%d,%d,%d],\"C\":123}~", id, c, r, g, b);
    commSendMsgToUartQueue(testServerMsg);
}

