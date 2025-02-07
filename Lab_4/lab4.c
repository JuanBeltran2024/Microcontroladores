#include <xc.h>
#include <stdlib.h>
#include <stdio.h>


#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF
#pragma config LVP = OFF //Programacion de bajo voltaje
#define _XTAL_FREQ 1000000


void comando_config(unsigned char c);
void dato(unsigned char d);
void inicio();
void letra(const char *le);
void puntero(unsigned char row, unsigned char col);
unsigned char teclado(void);
void interrupt ISR(void);
void limpiar();




void main(void) {
    
    ADCON1=15;
    
  // TRISC = 0b00000000;//RA3-RA5 EN-RW-RS
   TRISA = 0b00000000;//RA3-RA5 EN-RW-RS
   TRISB = 0b11110000;// teclado matricial
   TRISD = 0b00000000; //RD7-RD4 datos LCD
   TRISE = 0b11111000;
   LATB =  0b00000000;
   LATE =  0b00000000;
   RBPU = 0;
   unsigned char Tecla = 0;
   unsigned char estado = 0;
   unsigned char estado_2 = 0;
   unsigned char cuenta_objetivo = 0;
   unsigned char cuenta_restante = 0;
   char let[4];
   char let_2[4];
   

    
    
    inicio();    // Inicializa el LCD
    __delay_ms(200); 
    
    puntero(1,1);
    letra("bienvenido");  
     __delay_ms(5000);
     
     
    while(1){
        
    if (estado == 0 && estado_2 == 0){
     limpiar();
     puntero(1,1);
     letra("Ingrese cuenta");
     puntero(2,1);
     letra("(1-59) y OK");
     estado_2 = 1;
    }
    
    Tecla = teclado();
     
     if (Tecla > 0 && Tecla <= 9 && estado == 0) {
            cuenta_objetivo = cuenta_objetivo*10 + Tecla; 
            cuenta_restante = cuenta_objetivo;
            puntero(2,1);
            letra("             "); 
            puntero(2,1); 
            sprintf(let, "%d", cuenta_objetivo); 
            letra(let); 
            
        }if (Tecla == 10 && estado == 0) {
            if (cuenta_objetivo >= 1 && cuenta_objetivo <= 59) { // Valida el valor
                limpiar();
                puntero(1,1);
                letra("Cuenta guardada"); 
                estado = 1;
                __delay_ms(1000); 
                limpiar();
                puntero(1,1);
                letra("restante:");
                puntero(1,10);
                sprintf(let_2, "%d", cuenta_restante);
                letra(let_2);
                puntero(2,1);
                letra("Objetivo:");
                puntero(2,10);
                letra(let); 
                
            } else {
                limpiar();
                puntero(1, 1);
                letra("Error: fuera de"); // Mensaje de error
                puntero(2, 1);
                letra("rango (1-59)");
                __delay_ms(2000);
                cuenta_objetivo = 0; // Reinicia el valor
                limpiar();
                puntero(1,1);
                letra("Ingrese cuenta");
                puntero(2,1);
                letra("(1-59) y OK");
            }
            } else if (Tecla == 12) { 
            cuenta_objetivo = 0; 
            puntero(2, 1);
            letra("               ");
        }
    
    if (Tecla == 4 && cuenta_restante > 0){
        cuenta_restante = cuenta_restante - 1;
    __delay_ms(200);  
       puntero(1,10);
       sprintf(let_2, "%d", cuenta_restante);
       letra("  ");
       puntero(1,10);
       letra(let_2);
    }
    if (Tecla == 10 && cuenta_restante == 0 && estado == 1){
        cuenta_objetivo = 0;
        limpiar();
        puntero(1,1);
        letra("Cuenta objetiva");
        puntero(2,4);
        letra("CUMPLIDA!");
        __delay_ms(1000);
        estado = 0;
        estado_2 = 0;
        
            
    }
 

    __delay_ms(200);
}
  
}

    
    

void inicio(){  
comando_config(0x02);
comando_config(0x28);
comando_config(0x0F);
comando_config(0x06);
comando_config(0x01);
__delay_ms(2);
}

void limpiar(){
comando_config(0x01);
__delay_ms(2);
}

void comando_config(unsigned char c){
   // RC0 = 0;   // RW
    RA5 = 0;
    LATD =  (c >> 4);  
   // RC1 = 1;  // EN
    RA3 = 1;
    __delay_ms(1);
    // RC1 = 1;
    RA3 = 0;  // EN

    LATD = (c & 0x0F);
    // RC1 = 1;
    RA3= 1;  //EN
    __delay_ms(1);
    // RC1 = 1;
    RA3 = 0;  // EN
    
}

void dato(unsigned char d){
 
    // RC0 = 0;   // RW
    RA5 = 1;
    LATD =  (d >> 4);  
   // RC1 = 1;
    RA3= 1;  //EN
    __delay_ms(1);
    // RC1 = 1;
    RA3= 0;  //EN

    LATD = (d & 0x0F);   
    // RC1 = 1;
    RA3= 1;  //EN
    __delay_ms(1);
    // RC1 = 1;
    RA3= 0;  //EN
 
}

void letra(const char *le){
    while (*le){
        
      dato(*le++);
      
    }
} 
 
void puntero(unsigned char row, unsigned char col) {
    unsigned char pos;
    if (row == 1) {     
        pos = 0x80 + (col - 1);
    } else if (row == 2) {
        pos = 0xC0 + (col - 1);
    }
    comando_config(pos);
}  

unsigned char teclado(void) {
    LATB = 0b11110000;
    while(RB4==1 && RB5==1 && RB6==1 && RB7==1);
    
    LATB = 0b11111110;
    if (RB4 == 0) return 1;
    if (RB5 == 0) return 2;
    if (RB6 == 0) return 3;
    if (RB7 == 0) return 4;

    LATB = 0b11111101;
    if (RB4 == 0) return 5;
    if (RB5 == 0) return 6;
    if (RB6 == 0) return 7;
    if (RB7 == 0) return 8;

    LATB = 0b11111011;
    if (RB4 == 0) return 9;
    if (RB5 == 0) return 10; 
    if (RB6 == 0) return 11;
    if (RB7 == 0) return 12; 

    LATB = 0b11110111;
    if (RB4 == 0) return 13;
    if (RB5 == 0) return 14;
    if (RB6 == 0) return 15;
    if (RB7 == 0) return 16;

    return 0; 
}


