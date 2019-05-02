#include <string.h>
#include "app.h"
#include "lcd.h"
#include "main.h"
#include "uart.h"
#include "system.h"
#include "keypad.h"
#include "bluetooth.h"

COMMAND_BYTE command_byte;

void runControlUnit(void)
{
    int k;
    ////////////////////////////////
    /*------   KEYPAD/LCD   ------*/
    ////////////////////////////////
    if(showDataEn && bleData.dataReceived)
    { //Keypad(software) sets this: shows real-time data
        LCD_dataShow();        
    }
    if(btnEn)
    {//Keypad(hardware) set this: handles button logic
        btnPressed(btnEn);   
    }
    
    ////////////////////////////////
    /*------UART2(Bluetooth)------*/
    ////////////////////////////////
    if(bleData.dataAvailable)
    {//All used for printing
        uart_print("\r\n--Data Buffer--");
        for(k = 0; (k < 9) && (bleData.data[k][0] != '\0'); k++)
        {
            uart_print("\r\n");
            uart_print(bleData.data[k]);
            
        }
        uart_print("\r\n---------------\r\n");
    }
    else
    {
        k = 0;
        //uart_print("\x1b[2J");
        uart_print("--Bluetooth Modules--\r\n");
        while(bleData.foundBT[k][0] != '\0')
        {
            uart_print(bleData.foundBT[k++]);
            uart_print("\r\n");
        }
        uart_print("----Packet Buffer----\r\n");
        uart_print(bleData.packetBuf);
        uart_print("\r\n");
    } 
    if(!bleData.isTryingConn)
    {//If the connection is not started, start connecting
        bleData.isTryingConn = true;
        uart_print("\r\n--CONNECT FIRST--\r\n");
        delay(250);
        BLE_connect(1);
        command_byte = CONNECT_FIRST;
    }    
    if(bleData.dataReceived)
    {//Functions for when data is received on Bluetooth
        
    }
    
    if(BLE_searchStr("REBOOT",bleData.packetBuf))
    {//Searches for REBOOT in string and resets variables
        uart_print("\r\n--REBOOT--\r\n");
        delay(250);
        BLE_reboot();
    }
    if(BLE_searchStr("DISCONNECT",bleData.packetBuf))
    {//Searches for DISCONNECT and reboots system
        uart_print("\r\n--DISCONNECT--\r\n");
        delay(250);
        uart2_print("R,1\r");
    }
    
    ////////////////////////////////
    /*------    FSM CODE    ------*/
    ////////////////////////////////
    if(command_byte == IDLE)
    {//IDLE state so no connection is happening (waiting)
        
    }
    else if(command_byte == CONNECT_FIRST)
    {//Used to connect to the first node
        if(bleData.searchCmdEn && BLE_searchStr("CMD>", bleData.packetBuf))
        {//If command mode entered, start scanning
            bleData.searchCmdEn = false;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            uart_print("\r\n--Found CMD>--\r\n");
            delay(250);
            BLE_connect(2);            
        }
        if(bleData.searchMacEn)
        {
            BLE_findMAC(bleData.packetBuf);
        }
        if(bleData.searchMacEn && BLE_searchStr(MAC_FIRST, bleData.packetBuf))
        {//If the next node's MAC found connect to it
            bleData.searchMacEn = false;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            uart_print("\r\n--Found MAC--\r\n");
            delay(250);
            BLE_connect(3);
        }
        if(bleData.searchStreamEn && BLE_searchStr("STREAM_OPEN",bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchStreamEn = false;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            uart_print("\r\n--STREAM OPEN--\r\n");
            delay(250);
            bleData.isConnected = true;
            uart2_print("SEND_DATA,0");
        }
        if(bleData.isConnected)
        {
            if(BLE_parseData(bleData.packetBuf))
            {
                memset(bleData.packetBuf,'\0',PACKET_LEN);
                bleData.packetIndex = 0;
//                BLE_disconnect();
//                command_byte = IDLE;
                bleData.sensorCount++;
                command_byte = CONNECT_SECOND;
            }
        }
    }
    else if(command_byte == CONNECT_SECOND)
    {
        if(bleData.isConnected)
        {//Used for the first time this state is ran
            bleData.isConnected = false;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            uart_print("\r\n--REQUEST 2ND--\r\n");
            delay(250);
            uart2_print("SEND_DATA,1");
            BLE_disconnect();  
        }
        if(BLE_searchStr("STREAM_OPEN", bleData.packetBuf))
        {
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            uart_print("\r\n--STREAM OPEN 2--\r\n");
            delay(250);
            uart2_print("AOK");
        }
        if(BLE_parseData(bleData.packetBuf))
        {
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            uart_print("\r\n--RECEIVED 2ND--\r\n");
            delay(250);
            bleData.dataAvailable = true;
            bleData.sensorCount = 0;
            BLE_disconnect();
            command_byte = IDLE;
        }
    }
    else if(command_byte == CONNECT_THIRD)
    {
        
    }
    else if(command_byte == DISCONNECT)
    {
        
    }
    else if(command_byte == TEST)
    {
        if(BLE_searchStr("STREAM_OPEN", bleData.packetBuf))
        {
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
        }
    }
    
    bleData.dataReceived = false;
}