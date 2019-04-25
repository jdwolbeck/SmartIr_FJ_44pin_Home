/******************************************************************************/
// PIC with WiFi (ESP8266) ... Joshua Wolbeck ... ECE 461
/******************************************************************************/

/* Device header file */
#if defined (__XC16__)
    #include <xc.h>
#elif defined (__C30__)
    #if defined(__PIC24E__)
        #include <p24Fxxxx.h>
    #elif defined (__PIC24F__) || defined (__PIC24FK__)
        #include <p24Fxxxx.h>
    #elif defined(__PIC24H__)
        #include <p24Hxxxx.h>
    #endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "lcd.h"
#include "uart.h"
#include "system.h"
#include "keypad.h"
#include "bluetooth.h"
#include "initialization.h"
#include "configuration_bits.h"

int main() 
{
    //Used to ensure program doesn't infinitely loop unconnected to Bluetooth
    int antiStuck = 0;    
    InitApp();
    
    while(1)
    {
        delay(250);
        HB_LED = !HB_LED;
        
        //Keypad(software) sets this: shows realtime data
        if(showDataEn && bleData.dataReceived)
            LCD_dataShow();
        
        //UART2(Bluetooth) Interrupt sets this: when char receive
        if(bleData.dataReceived)
            BLE_update();
        
        //Keypad(hardware) set this: handles button logic
        if(btnEn)
            btnPressed(btnEn);
        
        //If stream !open: print out found addresses
        if(!bleData.isConnected)
        {
            if(!tryingConn)
            {
                tryingConn = true;
                BLE_connect(1);
            }
            int k = 0;
            while(bleData.foundBT[k][0] != '\0')
            {
                uart_print(bleData.foundBT[k++]);
                uart_print("\r\n");
            }
            uart_print("\r\n");
        }//if stream open: print out most recent data
        else
        {
            int k;
            for(k = 0; (k < 3) && (bleData.data[k][0] != '\0'); k++)
            {
                uart_print(bleData.data[k]);
                uart_print(" ");
            }
            uart_print("\r\n");
        }
        
        //Only used when stream !open and no data is being received
        //After about 20 seconds will reboot bluetooth in hopes to 
        //get out of situations where program is stuck. Used for robustness
        antiStuck++;
        if(antiStuck > 4*20 && !bleData.isConnected) //About 20 seconds
        {
            antiStuck = 0;
            uart2_print("R,1\r");
        }
        if(antiStuck > 100000)
            antiStuck = 0;
    }

    return 0;
}
