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
    InitApp();
 
    while(1) 
    {
        //loop every 250 ms and do logic
        if(bleData.dataAvailable && debug)
        {
            delay(500);
        }
        delay(50);
        //HB_LED = !HB_LED;
        runControlUnit();
    }

    return 0;
}
