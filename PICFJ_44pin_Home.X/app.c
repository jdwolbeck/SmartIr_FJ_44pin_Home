#include <string.h>
#include "app.h"
#include "lcd.h"
#include "main.h"
#include "uart.h"
#include "system.h"
#include "keypad.h"
#include "initialization.h"
#include "bluetooth.h"

COMMAND_BYTE command_byte;

void runControlUnit(void)
{
    int k;
    ////////////////////////////////
    /*------   KEYPAD/LCD   ------*/
    ////////////////////////////////
    if(btnEn)
    {//Keypad(hardware) set this: handles button logic
        btnPressed(btnEn);   
    }
    if(uart.packetReceived)
    {
        if(BLE_searchStr("START_WATER_", uart.buf))
        {
            //turn on water
            uart_parseGUIData(uart.buf);
            VALVE_EN = 1;
            T1CONbits.TON = 1;
            memset(uart.buf, '\0', PACKET_LEN_UART);
            uart.index = 0;
            uart.packetReceived = false;
        }
    }
    
    ////////////////////////////////
    /*------UART2(Bluetooth)------*/
    ////////////////////////////////
//    if(debug)
//    {
//        if(bleData.dataAvailable)
//        {
//            k = 0;
//            uart_print("\r\n--Data--\r\n");
//            while(bleData.data[k][0] != '\0')
//            {
//                uart_print(bleData.data[k++]);
//                uart_print("\r\n");
//            }
//        }
//    }
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
            bleData.isTryingConn = true;
            bleData.searchCmdEn = false;
            bleData.searchMacEn = false;
            bleData.searchStreamEn = false;
            if(!bleData.dataSent)
            {
                bleData.dataSent = true;
                delay(100);
                uart2_print("SEND_DATA,0");
            }
            
            if(BLE_parseData(bleData.packetBuf))
            {
                antiStuck = 0;
                if(debug)
                {
                    uart_print("\r\nData 1 received");
                }
                BLE_disconnect();
                prepare_variables(CONNECT_SECOND);
                command_byte = SEND_DATA;
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
            if(debug)
                uart_print("\r\nDISCONNECT1");
            antiStuck = 0;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            BLE_connect(4);
        }
        if(bleData.stuck)
        {
            if(debug)
            {
                uart_print("\r\nSTUCK1... BUF:\r\n");
                uart_print(bleData.packetBuf);
            }
            bleData.stuck = false;
            BLE_connect(1);
            delay(250);
            uart2_print("R,1\r");
            delay(10);
            if(BLE_searchStr("Err", bleData.packetBuf));
            {
                uart2_print("R,1\r");
            }
            while(BLE_searchStr("%REBOOT%", bleData.packetBuf));
            prepare_variables(CONNECT_FIRST);
            command_byte = CONNECT_FIRST;
        }
        if(antiStuck > TIMEOUT) //about 10 seconds
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
            bleData.isTryingConn = true;
            bleData.searchCmdEn = false;
            bleData.searchMacEn = false;
            bleData.searchStreamEn = false;
            if(!bleData.dataSent)
            {
                bleData.dataSent = true;
                uart2_print("SEND_DATA,0");
            }
            
            if(BLE_parseData(bleData.packetBuf))
            {
                antiStuck = 0;
                if(debug)
                    uart_print("\r\nData 2 received");
                delay(200);
                BLE_disconnect();
                prepare_variables(CONNECT_THIRD);
                command_byte = SEND_DATA;
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
            if(debug)
                uart_print("\r\nDISCONNECT2");
            antiStuck = 0;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            BLE_connect(4);
        }
        if(bleData.stuck)
        {
            if(debug)
            {
                uart_print("\r\nSTUCK2... BUF:\r\n");
                uart_print(bleData.packetBuf);
            }
            bleData.stuck = false;
            BLE_connect(1);
            delay(250);
            uart2_print("R,1\r");
            delay(10);
            if(BLE_searchStr("Err", bleData.packetBuf));
            {
                uart2_print("R,1\r");
            }
            while(BLE_searchStr("%REBOOT%", bleData.packetBuf));
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            prepare_variables(CONNECT_FIRST);
            command_byte = CONNECT_FIRST;
        }
        if(antiStuck > TIMEOUT) //about 10 seconds
        {
            bleData.stuck = true;
            antiStuck = 0;
        }
    }
    else if(command_byte == CONNECT_THIRD)
    {
        antiStuck++;
        if(bleData.isConnected)
        {
            bleData.isTryingConn = true;
            bleData.searchCmdEn = false;
            bleData.searchMacEn = false;
            bleData.searchStreamEn = false;
            if(!bleData.dataSent)
            {
                bleData.dataSent = true;
                uart2_print("SEND_DATA,0");
            }
            
            if(BLE_parseData(bleData.packetBuf))
            {
                antiStuck = 0;
                if(debug)
                    uart_print("\r\nData 3 received");
                delay(200);
                bleData.dataAvailable = true;
                BLE_disconnect();
                prepare_variables(CONNECT_FIRST);
                command_byte = SEND_DATA;
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
        if(bleData.searchMacEn && BLE_searchStr(MAC_THIRD, bleData.packetBuf))
        {
            antiStuck = 0;
            bleData.searchMacEn = false;
            BLE_connect(5);
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
            if(debug)
                uart_print("\r\nDISCONNECT3");
            antiStuck = 0;
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            BLE_connect(5);
        }
        if(bleData.stuck)
        {
            if(debug)
            {
                uart_print("\r\nSTUCK3... BUF:\r\n");
                uart_print(bleData.packetBuf);
            }
            bleData.stuck = false;
            BLE_connect(1);
            delay(250);
            uart2_print("R,1\r");
            delay(10);
            if(BLE_searchStr("Err", bleData.packetBuf));
            {
                uart2_print("R,1\r");
            }
            while(BLE_searchStr("%REBOOT%", bleData.packetBuf));
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
            prepare_variables(CONNECT_THIRD);
            command_byte = CONNECT_THIRD;
        }
        if(antiStuck > TIMEOUT) //about 10 seconds
        {
            bleData.stuck = true;
            antiStuck = 0;
        }
    }
    else if(command_byte == SEND_DATA)
    {
        HB_LED = 1;
        int i;
        if(bleData.sensorCount == 1)//First data received
        {
            i = 0;
            while(bleData.data[i][0] != '\0' && i < 3)
            {
                uart_print(bleData.data[i++]);
                uart_print(",");
                k++;
            }
            uart_print("1|");
            command_byte = CONNECT_SECOND;
        }
        else if(bleData.sensorCount == 2)//Second data received
        {
            i = 3;
            while(bleData.data[i][0] != '\0' && i < 6)
            {
                uart_print(bleData.data[i++]);
                uart_print(",");
                k++;
            }
            uart_print("2|");
            command_byte = CONNECT_THIRD;
        }
        else if(bleData.sensorCount == 0)//Third data received
        {
            i = 6;
            while(bleData.data[i][0] != '\0' && i < 9)
            {
                uart_print(bleData.data[i++]);
                uart_print(",");
                k++;
            }
            uart_print("3|");
            command_byte = CONNECT_FIRST;
        }
    }
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
        memset(bleData.packetBuf,'\0',PACKET_LEN);
        bleData.packetIndex = 0;
    }
    else if(c == CONNECT_SECOND)
    {
        bleData.sensorCount = 1;
        bleData.isTryingConn = false;
        bleData.searchCmdEn = true;
        bleData.searchMacEn = true;
        bleData.searchStreamEn = true;
        bleData.isConnected = false;
        bleData.dataSent = false;
        memset(bleData.packetBuf,'\0',PACKET_LEN);
        bleData.packetIndex = 0;
    }
    else if(c == CONNECT_THIRD)
    {
        bleData.sensorCount = 2;
        bleData.isTryingConn = false;
        bleData.searchCmdEn = true;
        bleData.searchMacEn = true;
        bleData.searchStreamEn = true;
        bleData.isConnected = false;
        bleData.dataSent = false;
        memset(bleData.packetBuf,'\0',PACKET_LEN);
        bleData.packetIndex = 0;
    }
}