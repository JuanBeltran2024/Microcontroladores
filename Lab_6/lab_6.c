#include <xc.h>
#include <stdio.h>



#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF
#pragma config LVP = OFF //Programacion de bajo voltaje
#define _XTAL_FREQ 1000000// Relos para uasar los retardos
#define TRIGGER RC0    // Define el pin de disparo en RC0
#define ECHO RC1 
 

void comando_config(unsigned char c);// Envio de comandos a la LCD /No retorna nada, recibe variables
void dato(unsigned char d);// No retorna nada, recibe variables
void dato_especial(unsigned char *caracter, unsigned char m);//Escribe caracteres especiales para ser enviados a dato /No retorna nada, recibe variables
void inicio();// Configuracion inicial LCD /No rotena, no recibe
void letra(const char *le);// Escribe cadenas de texto para ser enviadas a dato
void puntero(unsigned char row, unsigned char col);// Ubica el puntero en la posicion deseada/No retorna nada, recibe variables
void interrupt ISR(void);//Rutina de interrupciones
void limpiar();// Limpiar pantalla de visualizacion /No rotena, no recibe
void seg(unsigned char t);
void H_L_voltage_min(void);
void H_L_voltage_max(void);

//control motor

void iniciar_PWM(void);
void iniciar_ADC(void);
unsigned int Conversion(unsigned char canal);
void velocidad_motor(unsigned int resultado1);

//sensor ultrasonic
unsigned char MedirDistancia(void);
// Variables globales
int led = 3036;
unsigned char Tecla = 0;
unsigned char actividad = 0;  
unsigned char temporizador = 0;
unsigned char etimeout=0,ctimeout=0;





void main(void) {
    
    //inicio control motor
   iniciar_ADC();
   iniciar_PWM();
   
   TRISA = 0b00000001;//RA3-RA5 EN-RS, RW a tierra, RA4 encender y apagar LCD
   TRISB = 0b11110000;// Teclado matricial; RB0-RB3 filas (S), RB4-RB7 columnas (E) todos deben ser entradas para que funcione la interrupcion   
   TRISD = 0b00000000; //bton sumar y 7Seg D0-D3
   TRISE = 0b11110000;
   TRISC6 = 0;
   TRISC2 = 0;
   TRISC0 = 0;
   //TRISE = 0b11111000; Led RGB
   LATB =  0b00000000;// Display 7 seg
   LATE =  0b00001111;
   LATD = 0;
   LATA4 = 1;// luz de fondo
   LATC6 = 0;
   LATC2 = 0;
   LATC0 = 0;
   RBPU = 0;// Habilitacion de resistencias de pull up esta negado
   __delay_ms(100);
   // interrupciones
   //timer0
    TMR0=3036;// precarga
    T0CON=0b00000001;// Registro de timer0, apagado el timer0, modo temporizador, PS=4, 16 bits
    TMR0IF=0;//bandera
    TMR0IE=1;// Habilitacion local
    TMR0ON=1;// Timer0 encendido
    
    //timer1
    T1CON=0b10000000;
    
    //timer2 
    TMR2 = 0;
    TMR2ON = 1;
    
    //Teclado
    RBIF=0;// Borrar bandera de interrupcion 
    RBIE=1;// Habilitacion local de la interrupcion de teclado
    
    GIE=1;// activacion global interrupciones
   
   unsigned char estado = 0;// se activa si la cuenta objetivo esta dentro del rango establecido y pasa al tercer mensaje
   unsigned char estado_2 = 0;
   unsigned char cuenta_objetivo = 0;
   unsigned char cuenta_restante = 0;
   unsigned char cuenta = 0;
   unsigned char emergencia = 0;
   unsigned char fondo = 0;
   unsigned char state_bton = 0;//para antirrebote
   
   unsigned char col_led = 0;//registro cambio de color RGB
   unsigned char inicio_1 = 0;//para el estado de inicio
   
   //motor
   unsigned int resultado1 = 0;
   
   //Sensor ultrasonido
   unsigned int distancia = 0;
   
   // arreglo
   unsigned char caracter[8] = { 0b00000,0b01010,0b11111,0b11111,0b01110,0b00100,0b00000,0b00000};// Arreglo para mostrar el caracter especial (corazon)
   
   char let[3];// Arreglo para guardar las valores enteros de la cuenta objetivo que van a ser convertidos a texto
   char let_2[3];// Arreglo para guardar las valores enteros de la cuenta restante que van a ser convertidos a texto
   
   int i = 0;
   unsigned char j = 0;
 
   
   //Registros de inactividad
   unsigned char activo_10s = 0;  // Flag para controlar la luz de fondo
   unsigned char activo_20s = 0;  // Flag para controlar la suspensión
   unsigned char fondo = 0;
   

    
    inicio();    // Inicializa el LCD
    __delay_ms(200); 
    
    //-----------------Mensaje de bienvenida----------\\

   
        for(i=0;i<7;i++){
    puntero(1,i);
    letra("bienvenido");
    dato_especial(caracter,1);
     puntero(1,(10+i));
     dato(1); 
     __delay_ms(208);   
     limpiar();
     }
    
    for(i=7;i>=0;i--){
         puntero(1,i);
    letra("bienvenido");
    dato_especial(caracter,1);
     puntero(1,(10+i));
     dato(1); 
     __delay_ms(208);   
     limpiar();
     }
    
    
    
    
     
   //--------------------------------------------------\\ 
    
    while(1){
        
      resultado1 = Conversion(0);
      velocidad_motor(resultado1);
        
      if (distancia <= 8){actividad = 1;}  
  
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
    
     distancia = MedirDistancia();
     
     if (distancia > 8 ){
      state_bton = 0;
     }
     
        // -----------------7 segmentosss----------------------------------------------------
      LATD = cuenta; //corrimiento para utilizar los mas significativos valor del codificador del 7 seg
    //--------------------Estado inicio--------------------------\\

    if(cuenta == 0 && inicio_1 == 0){
    LATE = 0b00000111;
    }
     
    if (distancia <= 8 && state_bton == 0 && cuenta_restante > 0){
        cuenta_restante = cuenta_restante - 1;    
        cuenta = cuenta + 1; 
       puntero(1,10);
       sprintf(let_2, "%d", cuenta_restante);
       letra("  ");
       puntero(1,10);
       letra(let_2);
      state_bton = 1;
      inicio_1 = 1;
      __delay_ms(200);
      
   if (inicio_1 == 1){
        
    if (cuenta > 9){
        cuenta = 0;
        col_led = col_led + 1; 
        
         RA2 = 1;// Buzzer 0.25s
        __delay_ms(250);
         RA2 = 0;
    }
      if (col_led == 0){
             
            LATE =0b00000010;//Magenta
            
         }else if (col_led == 1){
         
             LATE =0b00000110;//Azul
             
         }else if (col_led == 2){
         
             LATE =0b00000100;// Cyan
             
             
         }else if (col_led == 3){
         
             LATE =0b00000101;//Verde
             
         }else if (col_led == 4){
         
             LATE =0b00000001;//Amarillo
             
         }else if (col_led == 5){
         
             LATE =0b000000000;// Blanco
             
         }else if (col_led > 5){
         
         col_led = 0;
        RA2 = 1;//Buzzer de 1s
         __delay_ms(800);
        RA2 = 0;
         
                 
         }

}
    }

          
    //-------------------------------------------------------\\
     
    if (Tecla == 14 && cuenta_restante == 0 && estado == 1){
        cuenta_objetivo = 0;
        cuenta = 0;
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
        
        RA4 = 1;
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
            RA4 = 0;         // Apagar luz de fondo
            activo_10s = 1; 
            fondo = 0;
        }

  
        if (temporizador >= 20 && activo_20s == 0) {
            RA4 = 0;  // Asegurar que la luz de fondo está apagada
            comando_config(0x08);  // Apagar pantalla LCD
            activo_20s = 1;  // Evitar que se vuelva a ejecutar
            fondo = 0;
        }

        // Si se detecta actividad, reactivar pantalla
        if (temporizador == 0 && activo_20s) {
      // Reiniciar el LCD
             comando_config(0x0C);
            __delay_ms(10);
            estado = 0;
            estado_2 = 0;
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
    LATD =  (c & 0xf0); // Corrimiento de 4 posiciones a la derecha para enviar los bits mas significativos a la LCD
       RA3 = 1;// E ejecucion de comando a la LCD comienzo del pulso
    __delay_ms(1);
       RA3 = 0;  // E Fin de la ejecucion fin del pulso

    LATD = (c << 4); // Envio de los bits menos significativos con la operacion AND
        RA3= 1;  //E
    __delay_ms(1);
    RA3 = 0;  // E    
}

void dato(unsigned char d){//Envio de datos a la LCD /d; Variabale que recibe datos
 
    RA5 = 1;// RS=1, Registro de datos
    LATD =  (d & 0xf0);  
    RA3= 1;  //E
    __delay_ms(1);
    RA3= 0;  //E
    LATD = (d << 4);   
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
void iniciar_PWM(void){
    PR2 = 249;
    CCPR1L = 0;
    T2CON=0b00000000; //Se configura preescalar de 1 y se mantiene temporalmente apagado
    CCP1CON = 0b00001100;//se configura modo PWM


}

void iniciar_ADC(void){
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001110;
    ADCON2 = 0b00001000;
}


unsigned int Conversion(unsigned char canal){
    if (canal>12)
        return 0;
    ADCON0 = (canal<<2)|0b00000001;
    GO = 1;
    while (GO == 1);
    return ADRESH;
}
void velocidad_motor(unsigned int resultado1){
    
     if(resultado1 >= 225) CCPR1L = 250;
       else if (resultado1 >= 204) CCPR1L = 200;
       else if (resultado1 >= 153) CCPR1L = 150;
       else if (resultado1 >= 102) CCPR1L = 100;
       else if (resultado1 >= 51) CCPR1L = 50;
       else if (resultado1 == 0) CCPR1L = 0;
        
}

unsigned char MedirDistancia(void){
  unsigned char aux=0;
  CCP2CON=0b00000100; //Ajustar CCP en modo captura con flanco de bajada
  TMR1=0;             //Iniciamos el timer1 en 0
  CCP2IF=0;           //Iniciar bandera CCPx en 0
  TRIGGER=1;          //Dar inicio al sensor
  __delay_us(10);
  TRIGGER=0;
  etimeout=1;         //Se habilita la condición de antibloqueo
  while(ECHO==0  && etimeout==1); //Se espera que el sensor empiece la
                      //medición o que pase el antibloqueo (aprox 2s)
  if(etimeout==0){    //Si el sensor no responde se retorna un 0
    return 0;
  }    
  TMR1ON=1;           //Se da inicio al timer1 o medición de tiempo
  while(CCP2IF==0 && TMR1IF==0);   //Espera a que la señal de ultrasonido regrese
  TMR1ON=0;           //Se da parada al timer 1 o medición de tiempo
  if(TMR1IF==1)       //Se comprueba que la medición del pulso del sensor no
    aux=255;          //exceda el rango del timer1, si es asi se limita a 255
  else{  
    if(CCPR2>=3556)  //Si el sensor excede 254cm se limita a este valor
      CCPR2=3556;
    aux=CCPR2/(14) ; //Se calcula el valor de distancia a partir del tiempo
  }
  return aux;         //Se retorna la medición de distancia obtenida
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
    
   if(TMR0IF == 1){
        TMR0IF=0;
        TMR0=led; //precarga     
        LATA1=LATA1^1; //conmutacion del pin C6(LED) con la operacion XOR
        temporizador = temporizador + 1;
        
         if(etimeout==1)   //Se cuenta cada segundo si esta habilitada la condición
            ctimeout++;     // de antibloqueo
         else
            ctimeout=0;     //Si no esta habilitada se reinicia la variable
          if(ctimeout>=2)   //Si la condición de antibloqueo excede dos cuentas
             etimeout=0; 
    }
   
}

