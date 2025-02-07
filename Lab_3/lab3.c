#include <xc.h>
#include <stdio.h>

#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF
#pragma config LVP = OFF //Programacion de bajo voltaje
#define ADCON1 = 15
#define _XTAL_FREQ 8000000


void main(void) {
    
    TRISB = 0b11110000;
    TRISD = 0b11110000;
    int numero = 0;
    int btn_state = 0;
    int btn_state2 = 0;
    int col_led = 0;
    int emergencia = 0;
    LATD = 0b00000111;
      
    
           
    while (1){
        
    if (RD6 == 1){
        
        LATD = 0b00000011;
        emergencia = 1;
        
    }if (RD6 == 0 &&  emergencia == 0){
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
             
            LATD =0b00000111;
            
         }else if (col_led == 1){
             
            LATD =0b00000010;
            
         }else if (col_led == 2){
         
             LATD =0b00000110;
             
         }else if (col_led == 3){
         
             LATD =0b00000100;
             
         }else if (col_led == 4){
         
             LATD =0b00000101;
             
         }else if (col_led == 5){
         
             LATD =0b00000001;
             
         }else if (col_led == 6){
         
             LATD =0b000000000;
             
         }else if (col_led > 6){
         
         col_led = 1;
                 
         }
    }
    
    }
    
    return; 
}

