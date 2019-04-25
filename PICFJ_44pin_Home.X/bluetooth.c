#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "main.h"
#include "uart.h"
#include "system.h"
#include "bluetooth.h"
#include "initialization.h"

BLE_DATA bleData;

void BLE_connect(int count)
{
    //This function is used to actually connect to the RN4871
    //(Bluetooth Module). Count is used form rest of program
    //to control what commands are being sent.
    
    if(count == 1)
    {
        uart2_print("$");
        delay(220);
        uart2_print("$$");  //CMD mode
    }
    else if(count == 2)
    {
        uart2_print("F\r"); //Search mode
    }
    else if(count == 3)
    {
        bleData.isPaired = true;
        uart2_print("C,0,801F12B58D2F\r"); //Connect to module
    }
}

void BLE_findMAC(char str[])
{
    //This function parses the packetBuf and looks for different MAC addresses.
    //It will compare found MAC addresses with our known node's MAC addresses.
    //After the desired address is found it is saved in bleData.foundBT[0-MAX]
    //Also initiates the final phase of Bluetooth connection process.
    
    int i = 0, j = 0, k, l;
    char temp[100];
    char temp2[100];

    while(str[i] != '\0' && str[i] != '%') // go through the string until first thing
    {
        i++;
    }
    
    while(str[i] != '\0')
    {
        k = 0;
        while(str[i] == '%')
        {
            i++;
        }
        
        while(str[i] != '\0' && str[i] != '%')
        {
            temp[k++] = str[i++];
        }
        
        //Do another read through of the device found and strip off MAC addr
        temp[k] = '\0';
        l = 0;
        while(temp[l] != ',')
        {
            temp2[l] = temp[l];
            l++;
        }
        temp2[l] = '\0';
        
        //Only save the MAC address of the found Bluetooth devices
        if(temp[0] != '\0' && temp[0] != '\r')
        {
            strcpy(bleData.foundBT[j++],temp2);
        }
    }
    
    k = 0;
    while(bleData.foundBT[k][0] != '\0')
    {
        if(!strcmp(bleData.foundBT[k],"801F12B58D2F"))
        {
            //strcpy(bleData.sensors[0],bleData.foundBT[k]);
            BLE_connect(3);
        }
        k++;
    }
}

bool BLE_searchStr(char key[], char str[])
{
    //This function will parse an input string (str) and
    //find the keyword within it. Returns true if found.
    
    char temp[strlen(key)];
    int i = strlen(key) - 1, j, k = 0;
    
    while(str[i++] != '\0')
    {
        for(j = 0; j < strlen(key); j++)
        {
            temp[j] = str[j+k];
        }
        temp[j] = '\0';
        if(!strcmp(temp,key))
            return true;
        k++;
    }
    return false;
}

void BLE_update(void)
{
    //This function is called with every program loop (about 250 ms).
    //The purpose is to take away the responsibility from the interrupt
    //to update the necessary functions when data is received from
    //the Bluetooth (UART 2).
    
    bool found = BLE_searchStr("CMD>", bleData.packetBuf);
    if(found && !bleData.isPaired)
    {
        BLE_connect(2);
    }
    
    if(!bleData.isPaired)
    {
        BLE_findMAC(bleData.packetBuf);
    }
    
    if(bleData.isConnected)
    {
        if(BLE_parseData(bleData.packetBuf))
        {
            memset(bleData.packetBuf,'\0',PACKET_LEN);
            bleData.packetIndex = 0;
        }
    }
    
    found = BLE_searchStr("DISCONNECT",bleData.packetBuf);
    if(found && !bleData.isConnected)
    {      
        uart_print("\r\n--DISCONNECT--\r\n");
        uart2_print("R,1\r");
    }

    found = BLE_searchStr("STREAM_OPEN",bleData.packetBuf);
    if(found && !bleData.isConnected)
    {
        memset(bleData.packetBuf,'\0',PACKET_LEN);
        bleData.packetIndex = 0;
        uart_print("\r\n--STREAM OPEN--\r\n");
        bleData.isConnected = true;
        bleData.isPaired = true;
    }
    
    found = BLE_searchStr("REBOOT",bleData.packetBuf);
    if(found)
    {
        InitBluetooth();
        uart_print("\r\n--REBOOT--\r\n");
        delay(1000);
    }
    
    bleData.dataReceived = false;
}

bool BLE_parseData(char str[])
{
    //This function is used after bluetooth connection is
    //established. This is used to determine when all three
    //data points have been received. Then takes these values
    //and puts them into their respective buffer. 
    //bleData.data[0-2] holds soil, lux, temp data.
    
    int i,j,k,n;
    int count=0;
    
    for(i = 0; bleData.packetBuf[i] != '\0'; i++)
    {
        if(bleData.packetBuf[i] == ',')
            count++;
    }
    
    if(count < 3)
        return false;
    
//    j/k : are the buffer index
//    i : represents a particular data
//    n : is the specific data's value index
    
    k = 0;
    for(i = 0; i < 3; i++)
    {
        n = 0;
        for(j = k; bleData.packetBuf[j] != ','; j++)
        {
            bleData.data[i][n++] = bleData.packetBuf[j];
            k++;
        }
        k++;
    }
    return true;
}