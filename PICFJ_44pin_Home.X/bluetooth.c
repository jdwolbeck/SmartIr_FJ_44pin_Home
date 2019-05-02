#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "app.h"
#include "main.h"
#include "uart.h"
#include "system.h"
#include "bluetooth.h"
#include "initialization.h"

BLE_DATA bleData;

char MAC_FIRST[20] = "801F12B58D2F";
//char MAC_FIRST[20] = "801F12BC47CD";
char MAC_SECOND[20] = "801F12BC47B6";
char MAC_THIRD[20] = "";

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
        uart2_print("C,0,");
        uart2_print(MAC_FIRST); //Connect to module
        uart2_print("\r");
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
    
    if(count != 3)
        return false;
//    j/k : are the buffer index
//    i : represents a particular data
//    n : is the specific data's value index
    k = 0;
    for(i = 3*bleData.sensorCount; i < 3 + 3*bleData.sensorCount; i++)//0; i < 3; i++)
    {
        n = 0;
        for(j = k; bleData.packetBuf[j] != ','; j++)
        {
            if(bleData.packetBuf[j] >= '0' && bleData.packetBuf[j] <= '9')
            {
                bleData.data[i][n++] = bleData.packetBuf[j];
            }
            k++;
        }
        k++;
    }
    return true;
}

void BLE_disconnect()
{
    uart_print("\r\nIN DC");
    BLE_connect(1);
    while(!BLE_searchStr("CMD>", bleData.packetBuf));
    uart2_print("K,1\r");
    while(!BLE_searchStr("DISCONNECT", bleData.packetBuf));
    uart2_print("---\r");
    while(!BLE_searchStr("END", bleData.packetBuf));
    memset(bleData.packetBuf,'\0',PACKET_LEN);
    bleData.packetIndex = 0;
    uart_print("\r\nOUT DC\r\n");
//    uart_print("\r\n--Disconnect--\r\n");
//    delay(250);
}

void BLE_reboot(void)
{
    bleData.isTryingConn = false;
    bleData.isConnected = false;
    bleData.searchCmdEn = true;
    bleData.searchMacEn = true;
    bleData.searchStreamEn = true;
    memset(bleData.packetBuf,'\0',PACKET_LEN);
    int i;
    for(i = 0; i < MAX; i++)
    {
        memset(bleData.foundBT[i],'\0',STR_LEN);
    }
    bleData.sensorCount = 0;
    bleData.packetIndex = 0;
}
