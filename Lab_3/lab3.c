#include <xc.h>
#include <stdio.h>
#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF
#pragma config LVP = OFF //Programacion de bajo voltaje
#define _XTAL_FREQ 1000000


void interrupt ISR(void); 

void main(void) {
    
    ADCON1 = 15;
    TRISB = 0b11110000;
    TRISD = 0b11100000;
    TRISE = 0b00000000;
    TRISA2 = 0;
    unsigned char numero = 0;
    unsigned char btn_state = 0;
    unsigned char btn_state2 = 0;
    unsigned char col_led = 0;
    unsigned char emergencia = 0;
    unsigned char inicio = 0; 
    LATE = 0b00000111;
    LATB = 0;
    
    //interrupciones
    TMR0=3036;
    T0CON=0b00000001;
    TMR0IF=0;
    TMR0IE=1;
    TMR0ON=1;
    RBIF=0;
    GIE=1;

      
    
           
    while (1){
    
    if(numero == 0 && inicio == 0){
    LATE = 0b00000111;
    if (RD7 == 0){
        btn_state = 0;
    }
    if (RD7== 1 && btn_state == 0){
        numero = numero + 1;
        btn_state = 1;
        inicio = 1;
    }
    }
        
    if (RD6 == 1){
        
        LATE = 0b00000011;
        emergencia = 1;
        
    }if (RD6 == 0 &&  emergencia == 0 && inicio == 1){
        LATB = numero;
        
     
 //------------logica boton Contador------------------------
    if (RD7 == 0){
        btn_state = 0;
    }
    if (RD7== 1 && btn_state == 0){
        numero = numero + 1;
        btn_state = 1;
      
  
    if (numero > 9){
    
        numero = 0;
        col_led = col_led + 1;  
        
        RA2 = 1;
        __delay_ms(250);
         RA2 = 0;
         
        
           
    }      
    }
 //-------------------------------------------------------------- 
//------------logica boton  reset------------------------
    if (RD5 == 1){
        btn_state2 = 1;
    }
    if (RD5 == 0 && btn_state2 == 1){
        numero = 0;
        btn_state2 = 0;
        col_led = 0;
    }
 //--------------------------------------------------------------
        
 //--------------------Logica color led RGB------------------------
            
          if (col_led == 0){
             
            LATE =0b00000010;
            
         }else if (col_led == 1){
         
             LATE =0b00000110;
             
         }else if (col_led == 2){
         
             LATE =0b00000100;
             
             
         }else if (col_led == 3){
         
             LATE =0b00000101;
             
         }else if (col_led == 4){
         
             LATE =0b00000001;
             
         }else if (col_led == 5){
         
             LATE =0b000000000;
             
         }else if (col_led > 5){
         
         col_led = 0;
        RA2 = 1;
         __delay_ms(800);
        RA2 = 0;
         
                 
         }
    }
    
    }
    
    return; 
}

void interrupt ISR(void){
        
    if(TMR0IF == 1){
        TMR0=3036; 
        TMR0IF=0;     
        LATD4=LATD4^1;
    }
}

