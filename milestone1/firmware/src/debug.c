#include "debug.h"


void halt(unsigned char outVal)
{
    while(1){
        // dbgOutputLoc(outVal);
    }
}

int writeToUART(char byte)
{
    if(!(DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART0_TransferStatus()) )
    {
                DRV_USART0_WriteByte(byte);
    }
    return 0;
}

int dbgUARTVal(unsigned char outVal)
{
    writeToUART(outVal);
    return 0;
}
