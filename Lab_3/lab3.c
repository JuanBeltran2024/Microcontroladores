#include <xc.h>
#include <stdio.h>

#pragma config FOSC = INTOSC_EC //Para oscilador de cristal mayor a 4MHz
#pragma config WDT = OFF
#pragma config LVP = OFF //Programacion de bajo voltaje
#define _XTAL_FREQ 1000000

void ADC_modulo(void);
unsigned int leer_ADC(void);
void interrupt ISR(void);

void main(void) {
    ADCON1 = 15;
    TRISB = 0b11110000;
    TRISE = 0b11110000;
    TRISA = 0b11111011;
    TRISD = 0b11100000;
    unsigned char numero = 0;
    unsigned char btn_state = 0;
    unsigned char btn_state2 = 0;
    int col_led = 0;
    int count6 = 0;
    
    unsigned char emergencia = 0;
    LATE = 0b00000111;

    //Interrupciones
    
   TMR0=3036;
    T0CON=0b00000001;
    TMR0IF=0;
    TMR0IE=1;
    TMR0ON=1;
    RBIF=0;
    RBIE=1;
    GIE=1;
      
    
           
    while (1){
        
     
        
    if (RD6 == 1){
        
        LATE = 0b00000011;
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
        RA2 = 1;
        __delay_ms(100);
        RA2 = 0;
        
           
    }
    if (col_led > 5 && count6 == 0){
        RA2 = 1;
        __delay_ms(400);
        RA2 = 0;
        count6 = 1;
        
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
             
            LATE =0b00000111;
            
         }else if (col_led == 1){
             
            LATE =0b00000010;
            
         }else if (col_led == 2){
         
             LATE =0b00000110;
             
         }else if (col_led == 3){
         
             LATE =0b00000100;
             
         }else if (col_led == 4){
         
             LATE =0b00000101;
             
         }else if (col_led == 5){
         
             LATE =0b00000001;
             
         }else if (col_led == 6){
         
             LATE =0b000000000;
             
         }else if (col_led > 6){
         
         col_led = 1;
         count6 = 0;
                 
         }
    }
    
    }
    
    return; 
}

void ADC_modulo(void){
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001110;
    ADCON2 = 0b10001010;
}

unsigned int leer_ADC(void){
    GO = 1;
    while(GO == 1){
        return ((ADRESH << 8) + ADRESL);
    }

}

void interrupt ISR(void){
 
   if(TMR0IF==1){
        TMR0IF=0;
        TMR0=3036;
        LATD4=LATD4^1;
    }
}

