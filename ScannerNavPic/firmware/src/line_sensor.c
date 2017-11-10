#include "line_sensor.h"
#include "communication_globals.h"
#include "motor_globals.h"

LINE_SENSOR_DATA line_sensorData;

void sendMsgToSensorQFromISR(struct sensorQueueData msg) {
    xQueueSendToBackFromISR(sensor_q, &msg, NULL);  
}

void sensorHandleIncomingMsg(struct sensorQueueData data) {
    // FIXME
}

// Harmony Driver TCS Color sensor drivers
// Initializes the color sensor
void DRV_TCS_Init(DRV_HANDLE i2c_handle, unsigned char waitTime, unsigned char gain){
    // Initialize the device
    unsigned char enableData;
    DRV_TCS_GetByte(i2c_handle, TCS_ENABLE_REGISTER, &enableData);
    DRV_TCS_SendByte(i2c_handle, TCS_ENABLE_REGISTER, (enableData | (TCS_ENABLE_PON_MASK | TCS_ENABLE_AEN_MASK)));
 
    // Set the wait time
    DRV_TCS_SendByte(i2c_handle, TCS_WAIT_TIMING_REGISTER, waitTime);
    
    // Set the ADC gain
    DRV_TCS_SendByte(i2c_handle, TCS_CONTROL_REGISTER, (gain & TCS_CONTROL_AGAIN_MASK));
}

// Configures the interrupt stuff for the color sensor
void DRV_TCS_IntConfig(DRV_HANDLE i2c_handle, unsigned short lowThreshold, unsigned short highThreshold, unsigned char persistence){
    // Set the interrupt thresholds
    DRV_TCS_SendByte(i2c_handle, TCS_LOW_THRESHOLD_LOW_REGISTER, (lowThreshold & 0x00ff));
    DRV_TCS_SendByte(i2c_handle, TCS_LOW_THRESHOLD_HIGH_REGISTER, ((lowThreshold & 0xff00) >> 8));
    DRV_TCS_SendByte(i2c_handle, TCS_HIGH_THRESHOLD_LOW_REGISTER, (highThreshold & 0x00ff));
    DRV_TCS_SendByte(i2c_handle, TCS_HIGH_THRESHOLD_HIGH_REGISTER, ((highThreshold & 0xff00) >> 8));
    
    // Set the persistence value
    DRV_TCS_SendByte(i2c_handle, TCS_PERSISTENCE_REGISTER, (persistence & TCS_PERSISTENCE_APERS_MASK));
}

// Send a byte to the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_SendByte(DRV_HANDLE i2c_handle, unsigned char address, unsigned char data){
    // Send buffer
    unsigned char send_buffer[2];
    send_buffer[0] = (address & TCS_REGISTER_ADDRESS_MASK) | (TCS_COMMAND_SELECT_MASK);
    send_buffer[1] = data;
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_Transmit(i2c_handle, TCS_I2C_ADDRESS << 1, send_buffer, 2, NULL);
    return buffer_handle;
}

// Send an address to the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_SendAddress(DRV_HANDLE i2c_handle, unsigned char address){
    address &= TCS_REGISTER_ADDRESS_MASK;
    address |= (TCS_COMMAND_SELECT_MASK);
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_Transmit(i2c_handle, TCS_I2C_ADDRESS << 1, &address, 1, NULL);
    return buffer_handle;
}

// Receive a byte from the color sensor
DRV_I2C_BUFFER_HANDLE DRV_TCS_GetByte(DRV_HANDLE i2c_handle, unsigned char address, unsigned char *data){
    address &= TCS_REGISTER_ADDRESS_MASK;
    address |= (TCS_COMMAND_SELECT_MASK);
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_TransmitThenReceive (i2c_handle, TCS_I2C_ADDRESS << 1, &address, 1, data, 1, NULL);
    return buffer_handle;
}

// Read all of the color data from the color sensors
DRV_I2C_BUFFER_HANDLE DRV_TCS_GetColors(DRV_HANDLE i2c_handle, unsigned char data[8], unsigned char address){
    address &= TCS_REGISTER_ADDRESS_MASK;
    address |= (TCS_COMMAND_SELECT_MASK);
    DRV_I2C_BUFFER_HANDLE buffer_handle = DRV_I2C_TransmitThenReceive (i2c_handle, TCS_I2C_ADDRESS << 1, &address, 1, data, 8, NULL);
    return buffer_handle;
}

// Handle all communication with the color sensor
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
                unsigned char testServerMsg[256];
                sprintf(testServerMsg, "ERROR OPENING THE I2C MODULE");
                commSendMsgToUartQueue(testServerMsg);
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
                unsigned char message[2];
                message[0] = (TCS_COMMAND_SELECT_MASK | TCS_ENABLE_REGISTER);
                message[1] = (TCS_COMMAND_AUTO_INCREMENT);
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
                
                generateMotorActions();
                
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
                //Send information
                if(COLOR_SENSOR_SERVER_TESTING == 1) {
                    if (ColorSensorNumber == COLOR_SENSOR_ID_1){
                        char buf[64];
                        sprintf(buf, "{\"SEQ\":%i, \"COLORS1\": { \"R\": %i, \"G\": %i, \"B\": %i }}!", outgoing_seq, r, g, b);
                        commSendMsgToUartQueue(buf);


                    }

                    else if (ColorSensorNumber == COLOR_SENSOR_ID_2){
                        char buf[64];
                        sprintf(buf, "{\"SEQ\":%i, \"COLORS2\": { \"R\": %i, \"G\": %i, \"B\": %i }}!", outgoing_seq, r, g, b);
                        commSendMsgToUartQueue(buf);


                    }
                }
                
                Nop();
                PreviousState[ColorSensorNumber-1] = HandlerState[ColorSensorNumber-1];
                HandlerState[ColorSensorNumber-1] = STATE_SEND_READ_COLOR_DATA;
            }
            break;
        }
    }
    return HandlerState[ColorSensorNumber-1];
}

#define FORWARD_STATE 0
#define REVERSE_STATE 1
#define DONE_STATE    2

int SCAN_STATE;

void generateMotorActions() {
    struct motorQueueData data;
    
    switch(SCAN_STATE) {
        case FORWARD_STATE:
        {
            if(leftOnTape()) {
                // generate turn action
                data.type = SENSOR;
                data.action = 2;
                data.speed = 2;
                data.dist = 1;
            }

            else if (rightOnTape()) {
                // generate turn action
                data.type = SENSOR;
                data.action = 3;
                data.speed = 2;
                data.dist = 1;
            }

            else {
                // generate forward action
                data.type = SENSOR;
                data.action = 0;
                data.speed = 4;
                data.dist = 1;
            }
            
            sendMsgToMotorQ(data);
            
            if(posX >= 75) {
                SCAN_STATE = DONE_STATE;
            }
            
            break;
        
        }
        case REVERSE_STATE:
        {
            if(leftOnTape()) {
                data.type = SENSOR;
                data.action = 2; // turn left
                data.speed = 2;
                data.dist = 1;
            }

            else if (rightOnTape()) {
                // generate turn action
                data.type = SENSOR;
                data.action = 3; // turn right
                data.speed = 2;
                data.dist = 1;
            }

            else {
                data.type = SENSOR;
                data.action = 1;
                data.speed = 4;
                data.dist = 1;
            }
            
            sendMsgToMotorQ(data);
            
            if(posX <= 0) {
                SCAN_STATE = FORWARD_STATE;
            }
            
            break;
        
        }
        case DONE_STATE:
        {
            // DO NOTHING
            break;
        }
        default: 
        {
            SCAN_STATE = DONE_STATE;
        }
    }
    
}

int leftOnTape() {
    unsigned int c;
    unsigned int r;
    unsigned int g;
    unsigned int b;
    
    c = ColorData1[0] | (ColorData1[1] << 8);
    r = ColorData1[2] | (ColorData1[3] << 8);
    g = ColorData1[4] | (ColorData1[5] << 8);
    b = ColorData1[6] | (ColorData1[7] << 8);
    
    // Need more calibration and testing
    if((r < 1000) || (g < 1000) || (b < 1000)) {
        return 1;
    }
    return 0;
    
}

int rightOnTape() {
    unsigned int c;
    unsigned int r;
    unsigned int g;
    unsigned int b;
    
    c = ColorData2[0] | (ColorData2[1] << 8);
    r = ColorData2[2] | (ColorData2[3] << 8);
    g = ColorData2[4] | (ColorData2[5] << 8);
    b = ColorData2[6] | (ColorData2[7] << 8);
    
    // Need more calibration and testing
    if((r < 1000) || (g < 1000) || (b < 1000)) {
        return 1;
    }
    
    return 0;
}

void LINE_SENSOR_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    line_sensorData.state = LINE_SENSOR_STATE_INIT;

    line_sensorData.handleI2C0 = DRV_HANDLE_INVALID;
    line_sensorData.handleI2C1 = DRV_HANDLE_INVALID;
    
}

void LINE_SENSOR_Tasks ( void )
{
    
    /* Check the application's current state. */
    switch ( line_sensorData.state )
    {

        /* Application's initial state. */
        case LINE_SENSOR_STATE_INIT:
        {
            bool appInitialized = true;
            sensor_q = xQueueCreate(32, sizeof (struct sensorQueueData));
       
            if (line_sensorData.handleI2C0 == DRV_HANDLE_INVALID)
            {
                line_sensorData.handleI2C0 = DRV_I2C_Open(LINE_SENSOR_DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
                line_sensorData.handleI2C1 = DRV_I2C_Open(LINE_SENSOR_DRV_I2C_INDEX_1, DRV_IO_INTENT_READWRITE);
                appInitialized &= (( DRV_HANDLE_INVALID != line_sensorData.handleI2C0 ) && ( DRV_HANDLE_INVALID != line_sensorData.handleI2C1 ));
            }
        
            if (appInitialized)
            {
                line_sensorData.state = LINE_SENSOR_STATE_SERVICE_TASKS;
            }
            break;
        }

        case LINE_SENSOR_STATE_SERVICE_TASKS:
        {
            
		    /* Run the state machine for servicing I2C */
            DRV_TCS_HandleColorSensor(line_sensorData.handleI2C0, COLOR_SENSOR_ID_1);
            DRV_TCS_HandleColorSensor(line_sensorData.handleI2C1, COLOR_SENSOR_ID_2);
            
            struct sensorQueueData rec;
            
            if(xQueueReceive(sensor_q, &rec, portMAX_DELAY))
            {
                //dbgOutputLoc(MOTOR_THREAD_RECVD);
                
                sensorHandleIncomingMsg(rec);
            }
        
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
