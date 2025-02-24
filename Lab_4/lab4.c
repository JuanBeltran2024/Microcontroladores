#include <xc.h>
#include <stdio.h>


#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF
#pragma config LVP = OFF //Programacion de bajo voltaje
#define _XTAL_FREQ 1000000


void comando_config(unsigned char c);
void dato(unsigned char d);
void dato_especial(unsigned char *caracter, unsigned char m);
void inicio();
void letra(const char *le);
void puntero(unsigned char row, unsigned char col);
unsigned char teclado(void);
void interrupt ISR(void);
void limpiar();

// Variables globales
unsigned char Tecla = 0;



void main(void) {
    
    ADCON1=15;

    
  // TRISC = 0b00000000;//RA3-RA5 EN-RW-RS
   TRISA = 0b00000000;//RA3-RA5 EN-RW-RS
   TRISB = 0b11110000;// teclado matricial
   TRISD = 0b00000000; //RD7-RD4 datos LCD
   TRISE = 0b11111000;
   LATB =  0b00000000;
   LATE =  0b00000000;
   LATD=0;
   RBPU = 0;
   __delay_ms(100);
   // interrupciones
   //timer0
    TMR0=3036;// precarga
    T0CON=0b00000001;
    TMR0IF=0;//bandera
    TMR0IE=1;
    TMR0ON=1;
    //Teclado
    RBIF=0;// bandera
    RBIE=1;
    
    GIE=1;// activacion global interrupciones
   
   unsigned char estado = 0;
   unsigned char estado_2 = 0;
   unsigned char cuenta_objetivo = 0;
   unsigned char cuenta_restante = 0;
   unsigned char emergencia = 0;
   unsigned char led = 1;
   
   // arreglo
   unsigned char caracter[8] = { 0b00000,0b01010,0b11111,0b11111,0b01110,0b00100,0b00000,0b00000};
   unsigned char caracter_2[8] = { 0b00000,0b01010,0b11111,0b11111,0b01110,0b00100,0b00000,0b00000};
   
   char let[4];
   char let_2[4];
   
   unsigned char i = 0;
   unsigned char j = 0;
    
    
    inicio();    // Inicializa el LCD
    __delay_ms(200); 
    
    //-----------------Mensaje de bienvenida----------\\

    for(j=0;j<2;j){
        i=0;
        j++;
        for(i=0;i<82;i++){
    puntero(1,i);
    letra("bienvenido");
    dato_especial(caracter,1);
     puntero(1,(10+i));
     dato(1); 
     __delay_ms(50);   
     limpiar();
     if(i == 16){
         i = 53;
     }
    }
    }
     
   //--------------------------------------------------\\ 
    
    while(1){
        
        
        
  
  //--------------------Parada de emergencia--------------------\\    
    if (Tecla == 10 && emergencia == 0){
        limpiar();
        puntero(1,1);
        letra("Parada de");
        puntero(2,1);
        letra("emergencia");
        emergencia = 1;
    }
  //-------------------------------------------------------------\\
  //--------------------Funcionamiento LCD--------------------\\
   
    if (emergencia == 0){
        
    //logica primer mensaje
    
    if (estado == 0 && estado_2 == 0){
        limpiar();
        puntero(1,1);
        letra("Ingrese cuenta");
        puntero(2,1);
        letra("(1-59) y OK");
        estado_2 = 1;
    }

    
   
    
    
     if (Tecla > 0 && Tecla <= 9 && estado == 0) {
            cuenta_objetivo = cuenta_objetivo*10 + Tecla; 
            cuenta_restante = cuenta_objetivo;
            puntero(2,1);
            letra("             "); 
            puntero(2,1); 
            sprintf(let, "%d", cuenta_objetivo);//transforma enteros a texto 
            letra(let); 
            Tecla = 0;
            
        }if (Tecla == 15 && estado == 0) {
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
                Tecla = 0;
                
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
                Tecla = 0;
            }
            } else if (Tecla == 11 && estado == 0) { 
            cuenta_objetivo = 0; 
            puntero(2, 1);
            letra("               ");
            Tecla = 0;
        }
    
    if (Tecla == 12 && cuenta_restante > 0 && estado == 1){
        cuenta_restante = cuenta_restante - 1;
    __delay_ms(200);  
       puntero(1,10);
       sprintf(let_2, "%d", cuenta_restante);
       letra("  ");
       puntero(1,10);
       letra(let_2);
       Tecla = 0;
    }
    if (Tecla == 15 && cuenta_restante == 0 && estado == 1){
        cuenta_objetivo = 0;
        limpiar();
        puntero(1,1);
        letra("Cuenta objetiva");
        puntero(2,4);
        letra("CUMPLIDA!");
        __delay_ms(1000);
        estado = 0;
        estado_2 = 0;
        Tecla = 0;
        
            
    }
 

    
      }

     
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

void dato_especial(unsigned char *caracter, unsigned char m){
    
    unsigned char i=0;
    comando_config(0x40 | (m*8));
    
  
    for(i = 0; i<8; i++){
        dato(caracter[i]);
    }
     comando_config(0x80);
    
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
/*
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

 */

void interrupt ISR(void){
    if(RBIF==1){
        if(PORTB!=0b11110000){
            Tecla=0;
            LATB=0b11111110;
            if(RB4==0) Tecla=1;
            else if(RB5==0) Tecla=2;
            else if(RB6==0) Tecla=3;
            else if(RB7==0) Tecla=10;
            else{
                LATB=0b11111101;
                if(RB4==0) Tecla=4;
                else if(RB5==0) Tecla=5;
                else if(RB6==0) Tecla=6;
                else if(RB7==0) Tecla=11;
                else{
                    LATB=0b11111011;
                    if(RB4==0) Tecla=7;
                    else if(RB5==0) Tecla=8;
                    else if(RB6==0) Tecla=9;
                    else if(RB7==0) Tecla=12;
                    else{
                        LATB=0b11110111;
                        if(RB4==0) Tecla=13;
                        else if(RB5==0) Tecla=0;
                        else if(RB6==0) Tecla=14;
                        else if(RB7==0) Tecla=15;
                    }
                }
            }
            LATB=0b11110000;
        }
        __delay_ms(100);
        RBIF=0;
    }
    if(TMR0IF==1){
        TMR0IF=0;
        TMR0=3036;
        LATD4=LATD4^1;
    }
}


