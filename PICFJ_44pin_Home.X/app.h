typedef enum{
    IDLE = 0x00,
    CONNECT_FIRST = 0x01,
    CONNECT_SECOND = 0x02,
    CONNECT_THIRD = 0x03,
    DISCONNECT = 0x04,
    TEST = 0x0F
} COMMAND_BYTE;

extern COMMAND_BYTE command_byte;

void runControlUnit(void);