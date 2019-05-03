#include <stdbool.h>
#define HB_LED LATBbits.LATB9
#define SIZE 10
//typedef enum
//{
//   RS_DIR2 = 5
//} GLOBAL;

//extern GLOBAL global;
extern bool debug;
extern int TIMEOUT;
extern int delay_value;
extern int currentMenu;
extern int btnEn;
extern int antiStuck;
extern bool autoEn;
extern bool showData1En;
extern bool showData2En;
extern bool showData3En;
extern bool tryingConn;
extern bool dataAvailable;
extern int flow;

extern int t1_ms;
extern int t1_seconds;
extern int t1_minutes;
extern int dur_seconds;
extern int dur_minutes;
