#define U1TX_REG RPOR3bits.RP7R     //Set RB7 as U1TX (3)   
#define U1RX_REG RPINR18bits.U1RXR  //Set RB8 as U1RX
#define U1TX_DIR TRISBbits.TRISB7   //Set RB7 (TX) as output
#define U1RX_DIR TRISBbits.TRISB8   //Set RB8 (RX) as input

#define U2TX_REG RPOR1bits.RP3R     //Set RB3 as U2TX (3)   
#define U2RX_REG RPINR19bits.U2RXR  //Set RB2 as U2RX
#define U2TX_DIR TRISBbits.TRISB3   //Set RB3 (TX) as output
#define U2RX_DIR TRISBbits.TRISB2   //Set RB2 (RX) as input

//UART 1
void uart_print(char[]);

//UART2
void uart2_print(char[]);