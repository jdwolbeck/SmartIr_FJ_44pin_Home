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
    
    // BLUETOOTH //
    if(bleData.dataAvailable)
    {
        k = 0;
        uart_print("\r\n--Data--\r\n");
        while(bleData.data[k][0] != '\0')
        {
            uart_print(bleData.data[k++]);
            uart_print("\r\n");
        }
    }
    if(BLE_searchStr("REBOOT", bleData.packetBuf))
    {
        BLE_reboot();
    }
    
    // FSM //
    if(command_byte == IDLE)
    {
        if(!bleData.isTryingConn)
        {
            prepare_variables(CONNECT_FIRST);
            command_byte = CONNECT_FIRST;
        }
    }
    else if(command_byte == CONNECT_FIRST)
    {
        antiStuck++;
        if(bleData.isConnected)
        {
            antiStuck = 0;
            bleData.isTryingConn = true;
            bleData.searchCmdEn = false;
            bleData.searchMacEn = false;
            bleData.searchStreamEn = false;
            if(!bleData.dataSent)
            {
                bleData.dataSent = false;
                uart2_print("SEND_DATA,0");
            }
            
            if(BLE_parseData(bleData.packetBuf))
            {
                antiStuck = 0;
                uart_print("\r\nData 1 received");
                BLE_disconnect();
                bleData.sensorCount++;
                prepare_variables(CONNECT_SECOND);
                command_byte = CONNECT_SECOND;
            }
        }
        if(!bleData.isTryingConn)
        {
            antiStuck = 0;
            bleData.isTryingConn = true;
            BLE_connect(1);
        }
        if(bleData.searchCmdEn && BLE_searchStr("CMD>", bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchCmdEn = false;
            BLE_connect(2);
        }
        if(bleData.searchMacEn && BLE_searchStr(MAC_FIRST, bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchMacEn = false;
            BLE_connect(3);
        }
        if(bleData.searchStreamEn && BLE_searchStr("STREAM_OPEN", bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchStreamEn = false;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            bleData.isConnected = true;
        }
        if(BLE_searchStr("DISCONNECT", bleData.packetBuf))
        {
            uart_print("\r\nDISCONNECT1");
            antiStuck = 0;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            BLE_connect(4);
        }
        if(bleData.stuck)
        {
            uart_print("\r\nSTUCK1");
            bleData.stuck = false;
            BLE_connect(1);
            delay(250);
            uart2_print("R,1\r");
            while(BLE_searchStr("%REBOOT%", bleData.packetBuf));
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            prepare_variables(CONNECT_FIRST);
            command_byte = CONNECT_FIRST;
        }
        if(antiStuck > (4*10)) //about 10 seconds
        {
            bleData.stuck = true;
            antiStuck = 0;
        }
    }
    else if(command_byte == CONNECT_SECOND)
    {
        antiStuck++;
        if(bleData.isConnected)
        {
            antiStuck = 0;
            bleData.isTryingConn = true;
            bleData.searchCmdEn = false;
            bleData.searchMacEn = false;
            bleData.searchStreamEn = false;
            if(!bleData.dataSent)
            {
                bleData.dataSent = false;
                uart2_print("SEND_DATA,0");
            }
            
            if(BLE_parseData(bleData.packetBuf))
            {
                antiStuck = 0;
                uart_print("\r\nData 2 received");
                delay(200);
                bleData.dataAvailable = true;
                BLE_disconnect();
                prepare_variables(CONNECT_FIRST);
                command_byte = CONNECT_FIRST;
            }            
        }
        if(!bleData.isTryingConn)
        {
            antiStuck = 0;
            bleData.isTryingConn = true;
            BLE_connect(1);
        }
        if(bleData.searchCmdEn && BLE_searchStr("CMD>", bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchCmdEn = false;
            BLE_connect(2);
        }
        if(bleData.searchMacEn && BLE_searchStr(MAC_SECOND, bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchMacEn = false;
            BLE_connect(4);
        }
        if(bleData.searchStreamEn && BLE_searchStr("STREAM_OPEN", bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchStreamEn = false;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            bleData.isConnected = true;
        }
        if(BLE_searchStr("DISCONNECT", bleData.packetBuf))
        {
            uart_print("\r\nDISCONNECT2");
            antiStuck = 0;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            BLE_connect(4);
        }
        if(bleData.stuck)
        {
            uart_print("\r\nSTUCK2");
            bleData.stuck = false;
            BLE_connect(1);
            delay(250);
            uart2_print("R,1\r");
            while(BLE_searchStr("%REBOOT%", bleData.packetBuf));
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            prepare_variables(CONNECT_FIRST);
            command_byte = CONNECT_FIRST;
        }
        if(antiStuck > (4*10)) //about 10 seconds
        {
            bleData.stuck = true;
            antiStuck = 0;
        }
    }
    
    ////////////////////////////////
    /*------UART2(Bluetooth)------*/
    ////////////////////////////////
//    if(bleData.dataAvailable)
//    {//All used for printing
//        uart_print("\r\n--Data Buffer--");
//        for(k = 0; (k < 9) && (bleData.data[k][0] != '\0'); k++)
//        {
//            uart_print("\r\n");
//            uart_print(bleData.data[k]);
//            
//        }
//        uart_print("\r\n---------------\r\n");
//    }
//    else
//    {
//        k = 0;
//        //uart_print("\x1b[2J");
//        uart_print("\r\n--Bluetooth Modules--\r\n");
//        while(bleData.foundBT[k][0] != '\0')
//        {
//            uart_print(bleData.foundBT[k++]);
//            uart_print("\r\n");
//        }
//        uart_print("---------------------\r\n");
////        uart_print("----Packet Buffer----\r\n");
////        uart_print(bleData.packetBuf);
////        uart_print("\r\n");
//    } 
    
//    if(BLE_searchStr("REBOOT",bleData.packetBuf))
//    {//Searches for REBOOT in string and resets variables
//        BLE_reboot();
//    }
//    
//    //////////////////////////////
//    /*------    FSM CODE    ------*/
//    //////////////////////////////
//    if(command_byte == SET_BOND)
//    {
//        if(!bleData.isTryingConn)
//        {
//            uart_print("\r\nIn SECURED\r\n");
//            bleData.isTryingConn = true;
//            BLE_connect(1);
//        } 
//        if(bleData.searchCmdEn && BLE_searchStr("CMD>", bleData.packetBuf))
//        {
//            bleData.searchCmdEn = false;
//            BLE_connect(2);
//        }
//        if(bleData.searchMacEn && BLE_searchStr(MAC_FIRST, bleData.packetBuf))
//        {
//            uart_print("\r\nMAC FOUND\r\n");
//            bleData.searchMacEn = false;
//            BLE_connect(3);
//        }
//        if(!bleData.searchStreamEn && BLE_searchStr("STREAM_OPEN", bleData.packetBuf))
//        {
//            uart_print("\r\nSTREAM OPEN\r\n");
//            bleData.searchStreamEn = false;
//            uart2_print("B\r");
//        }     
//        if(!bleData.bonded && BLE_searchStr("SECURE", bleData.packetBuf))
//        {
//            uart_print("\r\n--BONDED--\r\n");
//            bleData.bonded = true;
//            uart2_print("---\r");
//            delay(100);
//            BLE_disconnect();
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;            
//            bleData.isTryingConn = false;
//            command_byte = IDLE;
//        }
//    }
//    if(command_byte == IDLE)
//    {//IDLE state so no connection is happening (waiting)
//        if(!bleData.isTryingConn)
//        {//If the connection is not started, start connecting
//            bleData.isTryingConn = true;
//            BLE_connect(1);
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;
//            bleData.searchCmdEn = true;
//            bleData.searchSecuredEn = true;
//            bleData.isConnected = false;
//            command_byte = CONNECT_FIRST;
//            uart_print("\r\nCONNECT FIRST\r\n");
//        }    
//    }
//    else if(command_byte == CONNECT_FIRST)
//    {//Used to connect to the first node
//        if(bleData.searchCmdEn && BLE_searchStr("CMD>", bleData.packetBuf))
//        {//If command mode entered, start scanning
//            uart_print("\r\nFOUND CMD\r\n");
//            bleData.searchCmdEn = false;
//            BLE_connect(4);            
//        }
//        if(bleData.searchSecuredEn && BLE_searchStr("SECURED", bleData.packetBuf))
//        {
//            uart2_print("SEND_DATA,0");
//            bleData.searchSecuredEn = false;
//            bleData.isConnected = true;
//        }
//        if(bleData.searchMacEn && BLE_searchStr(MAC_FIRST, bleData.packetBuf))
//        {//If the next node's MAC found connect to it
//            bleData.searchMacEn = false;
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;
//            uart_print("\r\n--Found MAC--\r\n");
//            delay(250);
//            BLE_connect(3);
//        }
//        if(bleData.searchStreamEn && BLE_searchStr("STREAM_OPEN",bleData.packetBuf))
//        {
//            antiStuck = 0;
//            bleData.searchStreamEn = false;
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;
//            uart_print("\r\n--STREAM OPEN--\r\n");
//            delay(250);
//            bleData.isConnected = true;
//            uart2_print("SEND_DATA,0");
//        }
//        if(bleData.isConnected)
//        {
//            uart_print("\r\nCONNECTED\r\n");
//            if(BLE_parseData(bleData.packetBuf))
//            {
//                uart_print("\r\nDATA RECEIVED\r\n");
//                memset(bleData.packetBuf,'\0',PACKET_LEN);
//                bleData.packetIndex = 0;
//                bleData.isTryingConn = false;
//                BLE_disconnect();
//                command_byte = IDLE;
////                bleData.sensorCount++;
////                command_byte = CONNECT_SECOND;
//            }
//        }
//    }
//    else if(command_byte == CONNECT_SECOND)
//    {
//        if(bleData.isConnected)
//        {//Used for the first time this state is ran
//            bleData.isConnected = false;
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;
//            uart_print("\r\n--REQUEST 2ND--\r\n");
//            delay(250);
//            uart2_print("SEND_DATA,1");
//            BLE_disconnect();  
//        }
//        if(BLE_searchStr("STREAM_OPEN", bleData.packetBuf))
//        {
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;
//            uart_print("\r\n--STREAM OPEN 2--\r\n");
//            delay(250);
//            uart2_print("AOK");
//        }
//        if(BLE_parseData(bleData.packetBuf))
//        {
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;
//            uart_print("\r\n--RECEIVED 2ND--\r\n");
//            delay(250);
//            bleData.dataAvailable = true;
//            bleData.sensorCount = 0;
//            BLE_disconnect();
//            command_byte = IDLE;
//        }
//    }
//    else if(command_byte == CONNECT_THIRD)
//    {
//        
//    }
//    else if(command_byte == DISCONNECT)
//    {
//        
//    }
//    else if(command_byte == TEST)
//    {
//        if(BLE_searchStr("STREAM_OPEN", bleData.packetBuf))
//        {
//            memset(bleData.packetBuf,'\0',PACKET_LEN);
//            bleData.packetIndex = 0;
//        }
//    }
}

void prepare_variables(COMMAND_BYTE c)
{
    if(c == IDLE)
    {
        
    }
    else if(c == CONNECT_FIRST)
    {
        bleData.isTryingConn = false;
        bleData.searchCmdEn = true;
        bleData.searchMacEn = true;
        bleData.searchStreamEn = true;
        bleData.isConnected = false;
        bleData.dataSent = false;
        bleData.sensorCount = 0;
    }
    else if(c == CONNECT_SECOND)
    {
        bleData.isTryingConn = false;
        bleData.searchCmdEn = true;
        bleData.searchMacEn = true;
        bleData.searchStreamEn = true;
        bleData.isConnected = false;
        bleData.dataSent = false;
    }
}