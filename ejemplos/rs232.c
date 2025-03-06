//Fosc=1MHz
//BaudRate 9600 bps
#include<xc.h>
#define _XTAL_FREQ 1000000

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF


void Transmitir(unsigned char);

unsigned char Recibir(void);

void main(void){
    //unsigned char BufferR;
    TXSTA=0b00100100;
    RCSTA=0b10000000;        
    BAUDCON=0b00001000;        
    SPBRG=25;               //9600 bps
    while(1){
        Transmitir('H');
        Transmitir('o');
        Transmitir('l');
        Transmitir('a');
        Transmitir(' ');
        Transmitir('m');
        Transmitir('u');
        Transmitir('n');
        Transmitir('d');
        Transmitir('o');               
        __delay_ms(500);
        //BufferR=Recibir();       
    }
}

unsigned char Recibir(void){
    while(RCIF==0);
    return RCREG;
}

void Transmitir(unsigned char BufferT){
    while(TRMT==0);
    TXREG=BufferT;
}
