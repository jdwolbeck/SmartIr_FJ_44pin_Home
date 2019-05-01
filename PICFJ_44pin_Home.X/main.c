/******************************************************************************/
// PIC with WiFi (ESP8266) ... Joshua Wolbeck ... ECE 461
/******************************************************************************/

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "app.h"
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
    //int antiStuck = 0;    
    InitApp();
    
    while(1)
    {
        //loop every 250 ms and do logic
        delay(250);
        HB_LED = !HB_LED;
        runControlUnit();

        //Only used when stream !open and no data is being received
        //After about 20 seconds will reboot bluetooth in hopes to 
        //get out of situations where program is stuck. Used for robustness
//        antiStuck++;
//        if(antiStuck > 4*20 && !bleData.isConnected) //About 20 seconds
//        {
//            antiStuck = 0;
//            uart2_print("R,1\r");
//        }
//        if(antiStuck > 100000)
//            antiStuck = 0;
    }

    return 0;
}
