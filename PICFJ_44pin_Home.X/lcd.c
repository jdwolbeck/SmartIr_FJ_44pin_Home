#include <xc.h>
#include <string.h>
#include <stdbool.h>
#include "lcd.h"
#include "main.h"
#include "uart.h"
#include "system.h"
#include "bluetooth.h"

//////////////////////////////////////////////////////////////
/*                 Basic Operations                         */
//////////////////////////////////////////////////////////////

void LCD_begin(void)
{
    delay(15);
    
    //8-bit mode initialization
    //Function Set: (Command, -, -, 1, 8-bit, 2-line, 5x8, -, -)
    //LCD_write(0,0,0,1,1,1,0,0,0);
    
    //4-bit mode initialization
    //This starts up in 8-bit mode (required)
    LCD_write(0,0,0,1,1);
    delay(10);
    LCD_write(0,0,0,1,1);
    delay(5);
    LCD_write(0,0,0,1,1);
    delay(10);
    LCD_write(0,0,0,1,0);
    delay(5);
    //This actually sets it into 4-bit mode
    LCD_write(0,0,0,1,0);
    delay(10);
    LCD_write(0,1,0,0,0);
    delay(5);
    
    //Display/Cursor: (Command, - , -, -, -, 1, DisplayOn, CursorOn, BlinkingOn)
    //LCD_write(0,0,0,0,0,1,1,1,1);
    LCD_write(0,0,0,0,0); // Higher nibble
    LCD_write(0,1,1,1,1); // Lower nibble
    delay(5);
    
    //Entry Mode: (Command, -, -, -, -, -, 1, Increment(Right), No Shift))
    //LCD_write(0,0,0,0,0,0,1,1,0);
    LCD_write(0,0,0,0,0);
    LCD_write(0,0,1,1,0);
    delay(5);
}

void LCD_clear(void)
{
    //Clear Display
    //LCD_write(0,0,0,0,0,0,0,0,1);
    LCD_write(0,0,0,0,0);
    LCD_write(0,0,0,0,1);
    delay(5);
}

void LCD_home(void)
{
    //Return Home
    //LCD_write(0,0,0,0,0,0,0,1,0);
    LCD_write(0,0,0,0,0);
    LCD_write(0,0,0,1,0);
    delay(5);
}

bool LCD_moveCursor(int row, int col)
{
    if((row > 1 || row < 0) || (col > 15 || col < 0))
        return false;

    //8-bit address
    LCD_write(0,1,row,0,0);
    LCD_write(0,col&0x8,col&0x4,col&0x2,col&0x1);
    delay(5);
    return true;
}

void LCD_firstLine(void)
{
    //LCD_write(0,1,0,0,0,0,0,0,0);
    LCD_write(0,1,0,0,0);
    LCD_write(0,0,0,0,0);
    delay(5);
}

void LCD_secondLine(void)
{
    //LCD_write(0,1,1,0,0,0,0,0,0);
    LCD_write(0,1,1,0,0);
    LCD_write(0,0,0,0,0);
    delay(5);
}

void LCD_blink(bool state)
{
    if(state) //Display ON, Blinking ON
    {
        //LCD_write(0,0,0,0,0,1,1,1,1);
        LCD_write(0,0,0,0,0);
        LCD_write(0,1,1,1,1);
        delay(5);
    }
    else //Display ON, Cursor OFF
    {
        //LCD_write(0,0,0,0,0,1,1,0,0);
        LCD_write(0,0,0,0,0);
        LCD_write(0,1,1,0,0);
        delay(5);
    }
}

void LCD_display(char str[])
{       
    bool d7, d6, d5, d4;
    
    int i = 0;
    int j;
    while(str[i] != '\0')
    {
        for(j = 0; j < 2; j++)
        {
            if(j == 0)
            {
                d7 = str[i] & 0x80;
                d6 = str[i] & 0x40;
                d5 = str[i] & 0x20;
                d4 = str[i] & 0x10;
            }
            else
            {
                d7 = str[i] & 0x08;
                d6 = str[i] & 0x04;
                d5 = str[i] & 0x02;
                d4 = str[i++] & 0x01;
            }
            LCD_write(1,d7,d6,d5,d4);
            delay(5);
        }
    }
}

void LCD_write(bool rs, bool d7, bool d6, bool d5, bool d4)
{
    RS = rs;
    //RW = 0; //Unused
    D7 = d7;
    D6 = d6;
    D5 = d5;
    D4 = d4;
    
    EN = 1;
    EN = 0;
}

void LCD_read(bool rs, bool d7, bool d6, bool d5, bool d4)
{
    RS = rs;
    //RW = 1; //Unimplemented
    D7 = d7;
    D6 = d6;
    D5 = d5;
    D4 = d4;
    
    EN = 1;
    EN = 0;
}

//////////////////////////////////////////////////////////////
/*                    MENU CODE                             */
//////////////////////////////////////////////////////////////

void LCD_mainMenu(void)
{
    currentMenu = MAIN_MENU;
    LCD_clear();
    LCD_display("1.Water   2.Data");
    LCD_secondLine();
    LCD_display("3.View    4.Info");
    LCD_blink(0);
}

void LCD_waterMenu(void)
{
    currentMenu = WATER_MENU;
    LCD_clear();
    LCD_display("1.ON      2.OFF");
    LCD_secondLine();
    LCD_display("3.Auto    4.Back");
}

void LCD_dataMenu(void)
{
    currentMenu = DATA_MENU;
    LCD_clear();
    LCD_display("1.Node1  2.Node2");
    LCD_secondLine();
    LCD_display("3.Node3   4.Back");
}

void LCD_nodeMenu1(void)
{
    currentMenu = NODE_MENU_1;
    LCD_clear();
    LCD_display("1.Show    2.Hide");
    LCD_secondLine();
    LCD_display("3.N/A     4.Back");
}

void LCD_nodeMenu2(void)
{
    currentMenu = NODE_MENU_2;
    LCD_clear();
    LCD_display("1.Show    2.Hide");
    LCD_secondLine();
    LCD_display("3.N/A     4.Back");
}

void LCD_nodeMenu3(void)
{
    currentMenu = NODE_MENU_3;
    LCD_clear();
    LCD_display("1.Show    2.Hide");
    LCD_secondLine();
    LCD_display("3.N/A     4.Back");
}

//////////////////////////////////////////////////////////////
/*                  Extra Functions                         */
//////////////////////////////////////////////////////////////

void LCD_waterOn(void)
{
    LCD_secondLine();
    LCD_display("                ");
    LCD_secondLine();
    LCD_display("Watering Enabled");
    delay(3000);
    LCD_waterMenu();
}

void LCD_waterOff(void)
{
    LCD_secondLine();
    LCD_display("                ");
    LCD_secondLine();
    LCD_display("Water shut down!");
    delay(3000);
    LCD_waterMenu();
}

void LCD_waterAuto(void)
{
    autoEn = !autoEn;
    if(autoEn)
    {
        LCD_firstLine();
        LCD_display("                ");
        LCD_firstLine();
        LCD_display("Auto-water: ON");
        delay(3000);
        LCD_waterMenu();
    }
    else
    {
        LCD_firstLine();
        LCD_display("                ");
        LCD_firstLine();
        LCD_display("Auto-water: OFF");
        delay(3000);
        LCD_waterMenu();
    }
}

void LCD_dataShow1(void)
{
    if(bleData.data[2][0] != '\0')
    {
        char temp[2];
        temp[0] = 0xDF; temp[1] = '\0';
        LCD_clear();
        LCD_display("Soil  Lux   Temp");
        LCD_secondLine();
        LCD_display(bleData.data[0]);
        LCD_moveCursor(1, 6);
        LCD_display(bleData.data[1]);
        LCD_moveCursor(1, 11);
        LCD_display(bleData.data[2]);
        LCD_display(temp);
        LCD_display("F");
    }
    else
    {
        LCD_clear();
        LCD_display(MAC_FIRST);
        LCD_secondLine();
        LCD_display("Disconnected");
        showData1En = false;
        delay(1000);
        LCD_nodeMenu1();
    }
}

void LCD_dataShow2(void)
{
    if(bleData.data[5][0] != '\0')
    {
        char temp[2];
        temp[0] = 0xDF; temp[1] = '\0';
        LCD_clear();
        LCD_display("Soil  Lux   Temp");
        LCD_secondLine();
        LCD_display(bleData.data[3]);
        LCD_moveCursor(1, 6);
        LCD_display(bleData.data[4]);
        LCD_moveCursor(1, 11);
        LCD_display(bleData.data[5]);
        LCD_display(temp);
        LCD_display("F");
    }
    else
    {
        LCD_clear();
        LCD_display(MAC_SECOND);
        LCD_secondLine();
        LCD_display("Disconnected");
        showData2En = false;
        delay(1000);
        LCD_nodeMenu2();
    }
}

void LCD_dataShow3(void)
{
    if(bleData.data[2][0] != '\0')
    {
        char temp[2];
        temp[0] = 0xDF; temp[1] = '\0';
        LCD_clear();
        LCD_display("Soil  Lux   Temp");
        LCD_secondLine();
        LCD_display(bleData.data[6]);
        LCD_moveCursor(1, 6);
        LCD_display(bleData.data[7]);
        LCD_moveCursor(1, 11);
        LCD_display(bleData.data[8]);
        LCD_display(temp);
        LCD_display("F");
    }
    else
    {
        LCD_clear();
        LCD_display(MAC_THIRD);
        LCD_secondLine();
        LCD_display("Disconnected");
        showData3En = false;
        delay(1000);
        LCD_nodeMenu3();
    }
}

void LCD_infoMenu(void)
{
    LCD_clear();
    LCD_display("1=>Top 2=>Top-Md");
    LCD_secondLine();
    LCD_display("3=>Bot-Md 4=>Bot");
    delay(2500);
    LCD_mainMenu();
}

void LCD_bleShow(void)
{
    int j = 0, ind = 1;
    char index[3];
    index[0] = ind + '0';
    index[1] = '.';
    index[2] = '\0';
    
    while(bleData.foundBT[j][0] != '\0')
    {
        if(!j)
        {
            index[0] = ind + '0';
            LCD_clear();
            LCD_secondLine();
            LCD_display(index);
            LCD_display(bleData.foundBT[j]);
            j++;
            ind++; //Used for indexing the BT module
            delay(750);
        }
        else
        {
            LCD_clear();
            index[0] = ind - 1 + '0';
            LCD_display(index);
            LCD_display(bleData.foundBT[j-1]);
            LCD_secondLine();
            
            index[0] = ind + '0';
            LCD_display(index);
            LCD_display(bleData.foundBT[j]);
            j++;
            ind++;
            delay(750);
        }
    }
    LCD_clear();
    index[0] = ind - 1 + '0';
    LCD_display(index);
    LCD_display(bleData.foundBT[j-1]);
    delay(750);
            
    LCD_mainMenu();
}