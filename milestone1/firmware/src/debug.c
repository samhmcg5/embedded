#include "debug.h"

void halt(unsigned char outVal)
{
    while(1)
    {
        dbgOutputLoc(outVal);
    }
}

int writeToUART(unsigned char byte)
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

unsigned char charToASCII(unsigned char ch)
{
    switch(ch)
    {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        case 10:
            return 'A';
        case 11:
            return 'B';
        case 12:
            return 'C';
        case 13:
            return 'D';
        case 14:
            return 'E';
        case 15:
            return 'F';
        default:
            return 'x';
    }
}