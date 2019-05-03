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
                LCD_nodeMenu1();
            else if(btn == 2) // K2 (Top-MID) was pressed
                LCD_nodeMenu2();
            else if(btn == 3) // K3 (Bot-MID) was pressed
                LCD_nodeMenu3();
            else if(btn == 4) // K4 (Bot) was pressed
                LCD_mainMenu();
            break;
        case NODE_MENU_1:
            if(btn == 1)
            {
                LCD_dataShow1();
                showData1En = true;
            }
            else if(btn == 2)
            {
                showData1En = false;
                LCD_nodeMenu1();
            }
            else
                LCD_dataMenu();
            break;
        case NODE_MENU_2:
            if(btn == 1)
            {
                LCD_dataShow2();
                showData2En = true;
            }
            else if(btn == 2)
            {
                showData2En = false;
                LCD_nodeMenu2();
            }
            else
                LCD_dataMenu();
            break;
        case NODE_MENU_3:
            if(btn == 1)
            {
                LCD_dataShow3();
                showData3En = true;
            }
            else if(btn == 2)
            {
                showData3En = false;
                LCD_nodeMenu3();
            }
            else
            {
                LCD_dataMenu();
            }
            break;
        default:
            break;
    }
    
    btnEn = 0;
}