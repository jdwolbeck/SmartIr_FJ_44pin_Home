#include <xc.h>
#include <stdbool.h>

//Bluetooth properties

#define MAX 15
#define NUM_OF_SENSORS 3
#define DATA_LEN 5
#define READINGS 3
#define STR_LEN 500
#define PACKET_LEN 1024

typedef struct
{
    bool en;
    int packetIndex;
    int count;
    char foundBT[MAX][STR_LEN];
    char sensors[NUM_OF_SENSORS][STR_LEN];
    char packetBuf[PACKET_LEN];
    char data[READINGS][DATA_LEN];
    bool isPaired;
    bool isConnected;
    bool dataReceived;
} BLE_DATA;

extern BLE_DATA bleData;

void BLE_connect(int);
void BLE_findMAC(char[]);
bool BLE_searchStr(char[],char[]);
void BLE_update(void);
bool BLE_parseData(char[]);