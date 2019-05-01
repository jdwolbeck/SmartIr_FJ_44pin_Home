#include <xc.h>
#include "keypad.h"
#include "lcd.h"
#include "main.h"
#include "system.h"

void btnPressed(int btn)
{
    switch(currentMenu)
    {
        case MAIN_MENU:
            if(btn == 1) // K1 (Top) was pressed
                LCD_waterMenu();
            else if(btn == 2) // K2 (Top-MID) was pressed
                LCD_dataMenu();
            else if(btn == 3) // K3 (Bot-MID) was pressed
                LCD_bleShow();
            else if(btn == 4) // K4 (Bot) was pressed
                LCD_infoMenu();
            break;
        case WATER_MENU:
            if(btn == 1) // K1 (Top) was pressed
                LCD_waterOn();
            else if(btn == 2) // K2 (Top-MID) was pressed
                LCD_waterOff();
            else if(btn == 3) // K3 (Bot-MID) was pressed
                LCD_waterAuto();
            else if(btn == 4) // K4 (Bot) was pressed
                LCD_mainMenu();
            break;
        case DATA_MENU:
            if(btn == 1) // K1 (Top) was pressed
            {
                showDataEn = true;
            }
            else if(btn == 2) // K2 (Top-MID) was pressed
            {
                showDataEn = false;
                //delay(100); //To fix error with overwriting bottom line
                LCD_dataMenu();
            }
//            else if(btn == 3) // K3 (Bot-MID) was pressed
//                LCD_waterAuto();
            else if(btn == 4) // K4 (Bot) was pressed
            {
                showDataEn = false;
                //delay(100); //To fix error with overwriting bottom line
                LCD_mainMenu();
            }
            break;
        case WIFI_MENU:
            if(btn == 1) // K1 (Top) was pressed
                LCD_wifiShow();
            else if(btn == 2) // K2 (Top-MID) was pressed
                LCD_wifiConnectMenu();
            else if(btn == 3) // K3 (Bot-MID) was pressed
                LCD_wifiIsConnected();
            if(btn == 4) // K4 (Bot) was pressed
                LCD_mainMenu();
            break;
        case MORE_MENU:
            if(btn == 1) // K1 (Top) was pressed
                LCD_bleShow();
//            else if(btn == 2) // K2 (Top-MID) was pressed
//                LCD_waterOff();
            if(btn == 3) // K3 (Bot-MID) was pressed
                LCD_infoMenu();
            else if(btn == 4) // K4 (Bot) was pressed
                LCD_mainMenu();
            break;
        case WIFI_CONN_MENU:
            if(btn == 1) // K1 (Top) was pressed
                LCD_wifiSelect(1);
            else if(btn == 2) // K2 (Top-MID) was pressed
                LCD_wifiSelect(2);
            else if(btn == 3) // K3 (Bot-MID) was pressed
                LCD_wifiSelect(3);
            else if(btn == 4) // K4 (Bot) was pressed
                LCD_wifiMenu();
            break;
        default:
            break;
    }
    
    btnEn = 0;
}