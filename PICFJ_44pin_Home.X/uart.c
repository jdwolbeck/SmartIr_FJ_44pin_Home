#include <xc.h>
#include "uart.h"
#include "main.h"

UART_VARS uart;

void uart_print(char str[])
{
    int i = 0;
    while(str[i] != '\0')
    {
        while(!U1STAbits.TRMT);
        U1TXREG = str[i++];
    }
}

void  uart_parseGUIData(char str[])
{
    dur_seconds = 10;
    dur_minutes = 0;
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