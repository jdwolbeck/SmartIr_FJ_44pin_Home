#include <xc.h>
#include "uart.h"

void uart_print(char str[])
{
    int i = 0;
    while(str[i] != '\0')
    {
        while(!U1STAbits.TRMT);
        U1TXREG = str[i++];
    }
}

void uart2_print(char str[])
{
    int i = 0;
    while(str[i] != '\0')
    {
        while(!U2STAbits.TRMT);
        U2TXREG = str[i++];
    }
}