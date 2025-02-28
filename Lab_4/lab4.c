#include <xc.h>
#include <stdio.h>


#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF
#pragma config LVP = OFF //Programacion de bajo voltaje
#define _XTAL_FREQ 1000000// Relos para uasar los retardos
 

void comando_config(unsigned char c);// Envio de comandos a la LCD /No retorna nada, recibe variables
void dato(unsigned char d);// No retorna nada, recibe variables
void dato_especial(unsigned char *caracter, unsigned char m);//Escribe caracteres especiales para ser enviados a dato /No retorna nada, recibe variables
void inicio();// Configuracion inicial LCD /No rotena, no recibe
void letra(const char *le);// Escribe cadenas de texto para ser enviadas a dato
void puntero(unsigned char row, unsigned char col);// Ubica el puntero en la posicion deseada/No retorna nada, recibe variables
void interrupt ISR(void);//Rutina de interrupciones
void limpiar();// Limpiar pantalla de visualizacion /No rotena, no recibe

// Variables globales
unsigned char Tecla = 0;
unsigned char actividad = 0;  
unsigned char temporizador = 0;




void main(void) {
    
   ADCON1=15;// Apagar todas las funciones analogicas
   
   TRISA = 0b00000000;//RA3-RA5 EN-RS, RW a tierra, RA4 encender y apagar LCD
   TRISB = 0b11110000;// Teclado matricial; RB0-RB3 filas (S), RB4-RB7 columnas (E) todos deben ser entradas para que funcione la interrupcion   
   TRISD = 0b10000000; //RD3-RD0 datos LCD
   //TRISE = 0b11111000; Led RGB
   LATB =  0b00000000;// Display 7 seg
   //LATE =  0b00000000;
   LATD = 0;
   LATA4 = 0;// Revisar
   RBPU = 0;// Habilitacion de resistencias de pull up esta negado
   __delay_ms(100);
   // interrupciones
   //timer0
    TMR0=3036;// precarga
    T0CON=0b00000001;// Registro de timer0, apagado el timer0, modo temporizador, PS=4, 16 bits
    TMR0IF=0;//bandera
    TMR0IE=1;// Habilitacion local
    TMR0ON=1;// Timer0 encendido
    //Teclado
    RBIF=0;// Borrar bandera de interrupcion 
    RBIE=1;// Habilitacion local de la interrupcion de teclado
    
    GIE=1;// activacion global interrupciones
   
   unsigned char estado = 0;// se activa si la cuenta objetivo esta dentro del rango establecido y pasa al tercer mensaje
   unsigned char estado_2 = 0;
   unsigned char cuenta_objetivo = 0;
   unsigned char cuenta_restante = 0;
   unsigned char emergencia = 0;
   unsigned char fondo = 0;
   unsigned char state_bton = 0;//para antirrebote
   
   // arreglo
   unsigned char caracter[8] = { 0b00000,0b01010,0b11111,0b11111,0b01110,0b00100,0b00000,0b00000};// Arreglo para mostrar el caracter especial (corazon)
   
   char let[3];// Arreglo para guardar las valores enteros de la cuenta objetivo que van a ser convertidos a texto
   char let_2[3];// Arreglo para guardar las valores enteros de la cuenta restante que van a ser convertidos a texto
   
   unsigned char i = 0;
   unsigned char j = 0;
   
   //Registros de inactividad
   unsigned char activo_10s = 0;  // Flag para controlar la luz de fondo
   unsigned char activo_20s = 0;  // Flag para controlar la suspensión
   unsigned char fondo = 0;
    
    inicio();    // Inicializa el LCD
    __delay_ms(200); 
    
    //-----------------Mensaje de bienvenida----------\\

    for(j=0;j<2;j){
        i=0;
        j++;
        for(i=0;i<2;i++){
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
    if (Tecla == 12 && emergencia == 0){
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

    
   
    
    if (estado == 0){
     if (Tecla > 0 && Tecla <= 9) {
            cuenta_objetivo = cuenta_objetivo*10 + Tecla; 
            cuenta_restante = cuenta_objetivo;
            puntero(2,1);
            letra("             "); 
            puntero(2,1); 
            sprintf(let, "%d", cuenta_objetivo);//transforma enteros a texto 
            letra(let); 
            Tecla = 0;
            
        }if (Tecla == 15) {
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
            } else if (Tecla == 13) { 
            cuenta_objetivo = 0; 
            puntero(2, 1);
            letra("               ");
            Tecla = 0;
        }
    }

    if( estado == 1){
     if (Tecla == 11 ){
        cuenta_objetivo = 0;
        estado = 0;
        estado_2 = 0;
     }
     if (RD7 == 0 ){
      state_bton = 0;
     }
    if (RD7 == 0 && state_bton == 0 && cuenta_restante > 0){
        cuenta_restante = cuenta_restante - 1;
    __delay_ms(200);  
       puntero(1,10);
       sprintf(let_2, "%d", cuenta_restante);
       letra("  ");
       puntero(1,10);
       letra(let_2);
      state_bton == 1;
    }
    if (Tecla == 14 && cuenta_restante == 0 && estado == 1){
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
    
   //Logica de inactividad   
    if (actividad) {//verifica si actividad = 1
            
        if (activo_10s == 1){RA4 = !fondo;}
        if (Tecla == 10){
            fondo = !fondo;
            RA4 = fondo;
            __delay_ms(200);
        }
            temporizador = 0;
            actividad = 0;  // Restablecer flag de actividad
            activo_10s = 0;  // Reactivar la luz de fondo
            activo_20s = 0;  // Reactivar el sistema
            
            
        }
    
            if (temporizador >= 10 && activo_10s == 0) {
            RA4 = 1;         // Apagar luz de fondo
            activo_10s = 1; 
            fondo = 1;
        }

  
        if (temporizador >= 20 && activo_20s == 0) {
            RA4 = 1;  // Asegurar que la luz de fondo está apagada
            comando_config(0x08);  // Apagar pantalla LCD
            activo_20s = 1;  // Evitar que se vuelva a ejecutar
            fondo = 1;
        }

        // Si se detecta actividad, reactivar pantalla
        if (temporizador == 0 && activo_20s) {
            inicio();  // Reiniciar el LCD
            __delay_ms(200);
            
            estado == 0;
            estado_2 == 0;
        }
   }  
}

    
    

void inicio(){  // Configuracion inicial LCD
comando_config(0x02);
comando_config(0x28);
comando_config(0x0C);
comando_config(0x06);
comando_config(0x01);
__delay_ms(2);
}

void limpiar(){// Limpiar pantalla de visualizacion
comando_config(0x01);
__delay_ms(2);
}

void comando_config(unsigned char c){// Envio de comandos a la LCD /C;variable a la que se le cargan los datos
   
    RA5 = 0;// RS=0 seleccion de comando
    LATD =  (c >> 4); // Corrimiento de 4 posiciones a la derecha para enviar los bits mas significativos a la LCD
       RA3 = 1;// E ejecucion de comando a la LCD comienzo del pulso
    __delay_ms(1);
       RA3 = 0;  // E Fin de la ejecucion fin del pulso

    LATD = (c & 0b00001111); // Envio de los bits menos significativos con la operacion AND
        RA3= 1;  //E
    __delay_ms(1);
    RA3 = 0;  // E    
}

void dato(unsigned char d){//Envio de datos a la LCD /d; Variabale que recibe datos
 
    RA5 = 1;// RS=1, Registro de datos
    LATD =  (d >> 4);  
    RA3= 1;  //E
    __delay_ms(1);
    RA3= 0;  //E
    LATD = (d & 0b00001111);   
    RA3= 1;  //E
    __delay_ms(1);
    RA3= 0;  //E
 
}

void dato_especial(unsigned char *caracter, unsigned char m){// caracter funciona como apuntador, m; variabale que se le cargan los datos
    
    unsigned char i=0;
    comando_config(0x40 | (m*8));//enviamos el comando de escribir caracteres propios y hacemos operacion OR con el valor de m multiplicado 8 para que se posicione al inicio de cada fila    
    for(i = 0; i<8; i++){// Bucle
        dato(caracter[i]);//Se cargan los datos del arreglo caracter paa ser enviados a dato
    }
     comando_config(0x80);//Envial el comando de forzar el cursor al comienzo de la primera linea
    
}

void letra(const char *le){//Es una función que recibe un puntero a una cadena de caractereses (palabra), para ser enviada a dato
    while (*le){//verifica que el carácter apuntado por le no sea el carácter nulo si no finalizara el bucle
        
      dato(*le++);//obtiene el carácter actual en la posición apuntada por le,envía este carácter a dato y incrementa el puntero le, moviéndolo al siguiente carácter de la cadena.
      
    }
} 
 
void puntero(unsigned char row, unsigned char col) {// Ubica el puntero en la posicion deseada
    unsigned char pos;
    if (row == 1) {  //cursor debe ubicarse en la fila 1   
        pos = 0x80 + (col - 1);//Fuerza el cursor al inicio de la primera fila, ajusta la posicion de col para que inicie en 0.
    } else if (row == 2) { //cursor debe ubicarse en la fila 2
        pos = 0xC0 + (col - 1);//Fuerza el cursor al inicio de la segunda fila, ajusta la posicion de col para que inicie en 0.
    }
    comando_config(pos);//envia la posicion calculada a comando_config
}  


void interrupt ISR(void){
    
    if(RBIF==1){//verifica si se activo la bandera
      actividad = 1;
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
        temporizador = temporizador + 1;
    }
   
}


