#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "initialization.h"
#include "lcd.h"
#include "app.h"
#include "main.h"
#include "uart.h"
#include "system.h"
#include "bluetooth.h"

//Global Variables
bool debug = 0;
int TIMEOUT = 300;
int delay_value = 1000; //ms
int currentMenu = 0;
int antiStuck = 0;   
int btnEn = 0;
bool autoEn = false;
bool showData1En = false;
bool showData2En = false;
bool showData3En = false;
bool tryingConn = false;
bool dataAvailable = false;
int t1_ms = 0;
int t1_seconds = 0;
int t1_minutes = 0;
int dur_seconds = 0;
int dur_minutes = 0;
int flow = 0;

void InitApp()
{
    InitGPIO();
    InitUART();
    InitLCD();
    InitExtInt1();
    InitTimer();
    InitKeypad();
    InitBluetooth();
}

void InitGPIO()
{
    LATB = 0;
    //Set direction of RB pins (1 = input | 0 = output)
    TRISB = 0xFFFF;
    //Set pins as digital input pins
    ANSB = 0;
    ANSCbits.ANSC3 = 0;
    //Valve enable
    VALVE_EN_DIR = 0;
    HB_LED_DIR = 0;
}

void InitUART()
{
    memset(uart.buf, '\0', PACKET_LEN_UART);
    uart.index = 0;
    uart.packetReceived = false;
    
    //UART 1 (Regular/ESP)
    U1MODE = 0x0000;//(0x8008 & ~(1<<15));
    U1STA = 0x0000;
    
    U1TX_DIR = 0;   //Set RB7 (TX) as output
    U1RX_DIR = 1;   //Set RB8 (RX) as input 
    U1TX_REG = 3;   //Set RB7 as U1TX (3)   
    U1RX_REG = 8;   //Set RB8 as U1RX
    
    IFS0bits.U1RXIF = 0; //Clear RX interrupt Flag
    IPC2bits.U1RXIP = 3; //Priority of RX set to 3
    IEC0bits.U1RXIE = 1; //RX interrupt enabled   
    
    //Baud_Rate = 9600; Frequency = 4MHz; U1BRG = 25;
    U1BRG = (((FCY/BAUD_RATE)/16)-1); //Calculate baud rate registers value
    
    U1MODEbits.UARTEN = 1;  //UART1 enable
    U1STAbits.UTXEN = 1;    //UART1 TX Enable
    
    //UART 2 (Bluetooth))
    U2MODE = 0x0000;
    U2STA = 0x0000;
    
    U2TX_REG = 5;   //Set RB3 as U2TX (5)
    U2RX_REG = 2;   //Set RB2 as U2RX
    U2TX_DIR = 0;   //Set RB3 (TX) as output
    U2RX_DIR = 1;   //Set RB2 (RX) as input
    
    IFS1bits.U2RXIF = 0; //Clear the RX interrupt Flag
    IPC7bits.U2RXIP = 3; //Priority of RX set to 3
    IEC1bits.U2RXIE = 1; //RX interrupt enabled
    
    //Baud_Rate = 9600; Frequency = 4MHz; U2BRG = 25;
    U2BRG = (((FCY/BAUD_RATE)/16)-1); //Calculate baud rate registers value
    
    U2MODEbits.UARTEN = 1;  //UART2 enable
    U2STAbits.UTXEN = 1;    //TX enable
}

void InitLCD()
{
    // Set direction of all pins to output
    RS_DIR = 0;
    //RW_DIR = 0; //Unused (GND))
    EN_DIR = 0;
    
    D7_DIR = 0;
    D6_DIR = 0;
    D5_DIR = 0;
    D4_DIR = 0;
    
    //Set LCD and clear it
    LCD_begin();
    LCD_clear();
    LCD_blink(1);
    LCD_mainMenu();
}

void InitExtInt1()
{
    TRISBbits.TRISB4 = 1;
    RPINR0bits.INT1R = 4;
    
    INTCON2bits.INT1EP = 1; // falling edge
    IEC1bits.INT1IE = 1; // enable INT 1
    IFS1bits.INT1IF = 0; //  clear flag
}

void InitTimer()
{
    TMR1 = 0x0000;
    T1CON = 0;
    PR1 = 0x10;
    T1CONbits.TCKPS = 0x3;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
}

void InitKeypad()
{
    //Set keys as inputs
    K1_DIR = 1;
    K2_DIR = 1;
    K3_DIR = 1;
    K4_DIR = 1;
    
    //Enable Interrupt on negative going edge
    IOCNBbits.IOCNB10 = 1;
    IOCNBbits.IOCNB11 = 1;
    IOCNBbits.IOCNB12 = 1;
    IOCNBbits.IOCNB13 = 1;
    
    //Enable Pull-up resistors
    IOCPUBbits.CNPUB10 = 1;
    IOCPUBbits.CNPUB11 = 1;
    IOCPUBbits.CNPUB12 = 1;
    IOCPUBbits.CNPUB13 = 1;
    
    
    //Enabling IOC interrupt
    PADCONbits.IOCON = 1; // IOC interrupt enable
    IOCFB = 0; // Each bits interrupt flag
    IFS1bits.IOCIF = 0; // Interrupt flag
    IPC4bits.IOCIP = 3; // Interrupt priority
    IEC1bits.IOCIE = 1; // Global enable
}

void InitBluetooth(void)
{
    int i;
    for(i = 0; i < MAX; i++)
    {
        memset(bleData.foundBT[i],'\0',STR_LEN);
    }
    for(i = 0; i < NUM_OF_SENSORS; i++)
    {
        memset(bleData.sensors[i],'\0',STR_LEN);
    }
    for(i = 0; i < READINGS; i++)
    {
        memset(bleData.data[i],'\0',DATA_LEN);
    }
    memset(bleData.packetBuf,'\0',PACKET_LEN);
    bleData.en = true;
    bleData.isPaired = false;
    bleData.dataReceived = false;
    bleData.dataAvailable = false;
    bleData.isConnected = false;
    bleData.isTryingConn = false;
    bleData.searchCmdEn = true;
    bleData.searchMacEn = true;
    bleData.searchStreamEn = true;
    bleData.searchSecuredEn = true;
    bleData.bonded = false;
    bleData.sensorCount = 0;
    bleData.packetIndex = 0;
    bleData.dataSent = false;
    bleData.stuck = false;
    command_byte = IDLE;
}
