#include <stdbool.h>
#define HB_LED LATBbits.LATB9
#define SIZE 10
//typedef enum
//{
//   RS_DIR2 = 5
//} GLOBAL;

//extern GLOBAL global;
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
