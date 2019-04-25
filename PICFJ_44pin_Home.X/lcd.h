#include <xc.h>
#include <stdbool.h>

#define MAIN_MENU 0
#define WATER_MENU 1
#define DATA_MENU 2
#define WIFI_MENU 3
#define MORE_MENU 4
#define WIFI_CONN_MENU 5

#define RS LATCbits.LATC8
//#define RW LATBbits.LATB5
#define EN LATCbits.LATC9
#define D7 LATBbits.LATB15
#define D6 LATBbits.LATB14
#define D5 LATAbits.LATA7
#define D4 LATAbits.LATA10

void LCD_begin(void);
void LCD_clear(void);
void LCD_home(void);
bool LCD_moveCursor(int, int);
void LCD_firstLine(void);
void LCD_secondLine(void);
void LCD_blink(bool);
void LCD_display(char[]);
void LCD_write(bool,bool,bool,bool,bool);
void LCD_read(bool,bool,bool,bool,bool);

void LCD_mainMenu(void);
void LCD_waterMenu(void);
void LCD_dataMenu(void);
void LCD_wifiMenu(void);
void LCD_moreMenu(void);
void LCD_wifiConnectMenu(void);

void LCD_waterOn(void);
void LCD_waterOff(void);
void LCD_waterAuto(void);
void LCD_dataShow(void);
void LCD_wifiSelect(int);
void LCD_wifiShow(void);
void LCD_wifiIsConnected(void);
void LCD_infoMenu(void);
void LCD_bleShow(void);