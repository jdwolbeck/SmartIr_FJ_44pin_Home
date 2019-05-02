#include <xc.h>
#include <stdbool.h>

//Bluetooth properties
extern char MAC_THIS[20];
extern char MAC_FIRST[20];
extern char MAC_SECOND[20];
extern char MAC_THIRD[20];

#define MAX 15
#define NUM_OF_SENSORS 3
#define DATA_LEN 5
#define READINGS 6
#define STR_LEN 500
#define PACKET_LEN 1024

typedef struct
{
    bool en;
    int packetIndex;
    int sensorCount;
    char foundBT[MAX][STR_LEN];
    char sensors[NUM_OF_SENSORS][STR_LEN];
    char packetBuf[PACKET_LEN];
    char data[READINGS][DATA_LEN];
    bool searchCmdEn;
    bool searchMacEn;
    bool searchStreamEn;
    bool isPaired;
    bool isConnected;
    bool dataAvailable;
    bool dataReceived;
    bool isTryingConn;
    bool bonded;
    bool stuck;
    bool dataSent;
    bool searchSecuredEn;
} BLE_DATA;

extern BLE_DATA bleData;
extern char MAC_FIRST[20];
extern char MAC_SECOND[20];
extern char MAC_THIRD[20];

void BLE_connect(int);
void BLE_findMAC(char[]);
bool BLE_searchStr(char[],char[]);
bool BLE_parseData(char[]);
void BLE_disconnect(void);
void BLE_reboot(void);