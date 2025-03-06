
#include <xc.h>

#define _XTAL_FREQ 1000000
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#define TRIGGER RC0    // Define el pin de disparo en RC0
#define ECHO RC1       // Define el pin de eco en RC1

void iniciar_PWM(void);
void iniciar_ADC(void);
unsigned int Conversion(unsigned char canal);
void velocidad_motor(unsigned int resultado1);
unsigned char MedirDistancia(void);

void interrupt ISR(void);

//variable global
unsigned char etimeout=0,ctimeout=0;

void main(void) {
    
    TRISC2 = 0;
    TRISC0 = 0;
    TRISA2 = 0;
    
    
    LATA2 = 0;
    LATC0 = 0;
    
    T0CON=0b00001000;
     GIE=1;
    T1CON=0b10000000;
      TMR0IF=0;           //Se configura la interrupción del Timer0
  TMR0IE=1;
    TMR0ON=1; 
    
    //PWM
    iniciar_PWM();
    //Timer
    TMR2 = 0;
    TMR2ON = 1;
    
    //ADC
    iniciar_ADC();
    
    
    //Variable
    unsigned int resultado1 = 0; 
    unsigned int distancia = 0;
    
    while(1){
        resultado1 = Conversion(0);
        velocidad_motor(resultado1);
        distancia = MedirDistancia();
        if (distancia <= 8 ){
            RA2 = 1;
            __delay_ms(200);
            RA2 = 0;    
            __delay_ms(200);
        }
        
        
        
    }
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
    
  TMR0IF=0;
  TMR0=3036;

  if(etimeout==1)   //Se cuenta cada segundo si esta habilitada la condición
    ctimeout++;     // de antibloqueo
  else
    ctimeout=0;     //Si no esta habilitada se reinicia la variable
  if(ctimeout>=2)   //Si la condición de antibloqueo excede dos cuentas
    etimeout=0;     //se coloca la habilitación del antibloqueo en 0
}
