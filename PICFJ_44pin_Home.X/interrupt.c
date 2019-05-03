/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>
#include <stdio.h>
#include "uart.h"
#include "lcd.h"
#include "main.h"
#include <string.h>
#include "system.h"
#include "keypad.h"
#include "bluetooth.h"
#include "initialization.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
    char RXchar = U1RXREG;
    uart.buf[uart.index++] = RXchar;
    uart.buf[uart.index] = '\0';
    if(RXchar == '|')
    {
        uart.packetReceived = true;
    }
    IFS0bits.U1RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
    char RXchar = U2RXREG;
    
    if(RXchar != '\0')
    {
        if(bleData.packetIndex == (PACKET_LEN - 1))
            bleData.packetIndex = 0;
        bleData.packetBuf[bleData.packetIndex++] = RXchar;
        bleData.packetBuf[bleData.packetIndex] = '\0';
        bleData.dataReceived = true;
    }

    IFS1bits.U2RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _IOCInterrupt(void)
{
    if(IOCFBbits.IOCFB13) // K1 (Top) was pressed
        btnEn = 1;
    else if(IOCFBbits.IOCFB12) // K2 (Top-MID) was pressed
        btnEn = 2;
    else if(IOCFBbits.IOCFB11) // K3 (Bot-MID) was pressed
        btnEn = 3;
    else if(IOCFBbits.IOCFB10) // K4 (Bot) was pressed
        btnEn = 4;
    
    IOCFB = 0; // Clear individual flags
    IFS1bits.IOCIF = 0; // Interrupt flag
}

void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
    IFS1bits.INT1IF = 0; //interrupt flag
    if(VALVE_EN)
    {
        flow++;
    }
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    HB_LED = 1;
    TMR1 = 0x0000;
    t1_ms++;
    
    //count seconds
    if(t1_ms == 999)
    {
        if(t1_seconds == 59)
        {
            t1_seconds = 0;
            t1_minutes++;
        }
        else
        {
            t1_seconds++;
        }
        t1_ms = 0;
    }
    
    if(t1_seconds == dur_seconds && t1_minutes == dur_minutes)
    {
        HB_LED = 0;
        VALVE_EN = 0;
        t1_ms = 0;
        t1_seconds = 0;
        t1_minutes = 0;
        T1CONbits.TON = 0;
    }
}
