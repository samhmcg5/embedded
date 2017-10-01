#include "debug.h"

void halt(unsigned char outVal)
{
    while(1)
    {
        dbgOutputLoc(outVal);
    }
}

// blocking debug call
int writeToUART(unsigned char byte)
{
    if(!(DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART0_TransferStatus()) )
    {
        DRV_USART0_WriteByte(byte);
        return 0;
    }
    else
        return 1;
}

// blocking debug call
int dbgUARTVal(unsigned char outVal)
{
    return writeToUART(outVal);
}

void dbgOutputVal(unsigned char outVal)
{ 
    //TRISECLR = 0x00FF;
    //ODCECLR = 0x00FF;
    //LATECLR = 0x00FF;
    //LATESET = outVal; 
    
    TRISGCLR = 0x01C0;
    TRISDCLR = 0x0940;
    TRISFCLR = 0x0003;
    
    ODCGCLR = 0x01C0;
    ODCDCLR = 0x0940;
    ODCFCLR = 0x0003;
    
    LATGCLR = 0x01C0;
    LATDCLR = 0x0940;
    LATFCLR = 0x0003;
    
    LATGSET = (0x0000 | outVal) << 6;
    LATDSET = (outVal & 0x20) << 1;
    LATDSET = (outVal & 0x10) << 4;
    LATDSET = (outVal & 0x08) << 8;
    LATFSET = (outVal & 0x80) >> 7;
    LATFSET = (outVal & 0x40) >> 5;
}           

void dbgOutputLoc(unsigned char outVal)
{

        TRISECLR = 0x00FF;
        ODCECLR  = 0x00FF;
        
        LATECLR = 0x00FF;
        LATESET = outVal;
}

void writeUARTString(char * str, int len)
{
    int i;
    for (i=0; i<len; i++)
    {
        dbgUARTVal(str[i]);
    }
}