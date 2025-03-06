#include<xc.h>
#include<stdio.h>
#define _XTAL_FREQ 1000000
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma config LVP=OFF
#define TRIGGER RC0
#define ECHO RC1
void Transmitir(unsigned char);
void putch(char data);
unsigned char MedirDistancia(void);
void interrupt ISR(void);
unsigned char etimeout=0,ctimeout=0;
void main(void){
  //unsigned int t;
  unsigned char d;
  //OSCCON=0b01110000;  //oscilador interno se ajusta a 8 MHz
  //__delay_ms(1);      //Se espera que se estabilice el oscilador interno
  TRISC0=0;           //Se ajusta el pin de Trigger como salida
  TRISD0=0;           //Se ajusta una salida para un led de prueba
  LATC0=0;            //Se ajustan los valores inciales de las salidas
  LATD0=0;
  TXSTA=0b00100100;   //Se configura la transmisión serial RS232
  RCSTA=0b10000000;   //a 9600 bps para el envio de la medición de la
  BAUDCON=0b00001000; //distancia
  SPBRG=25;
  TMR0=3036;          //Se configura el Timer0 para interrupción de
  T0CON=0b00001000;   //sobreflujo cada segundo
  T1CON=0b10000000;   //Ajuste de timer1: prescaler 1
  TMR0IF=0;           //Se configura la interrupción del Timer0
  TMR0IE=1;
  GIE=1;              //Se habilitan las interrupciones
  TMR0ON=1;           //Se da inicio al Timer0
  while(1){
    while(1){
      d=MedirDistancia();   //Se realiza la medición de distancia
      if(d==0)              //Si la medida es 0 es por error del sensor
        printf("Falla en el sensor\r");
      else                  //Si no hay error se transmite la distancia
        printf("La distancia medida es: %d cm\r",d);
      __delay_ms(1000);     //Se repite el proceso cada segundo
    }
    //t=0;                //Iniciamos medición de tiempo en 0
    CCP2CON=0b00000100; //Ajustar CCP en modo captura con flanco de bajada
    TMR1=0;             //Iniciamos el timer1 en 0
    CCP2IF=0;           //Iniciar bandera CCPx en 0
    TRIGGER=1;          //Dar inicio al sensor
    __delay_us(10);
    TRIGGER=0;
    while(ECHO==0);
    /*
    while(ECHO==1){
      __delay_us(10);
      t++;
    }
    d=t/5.8;*/
    TMR1ON=1;           //Se da inicio al timer1 o medición de tiempo
    while(CCP2IF==0);   //Espera a que la señal de ultrasonido regrese
    TMR1ON=0;           //Se da parada al timer 1 o medición de tiempo
    d=CCPR2/58;         //213 => 2 1 3
    Transmitir(d/100 + 48);
    Transmitir((d%100)/10 + 48);
    Transmitir(d%10 + 48);
    Transmitir('\r');
    __delay_ms(1000);    
  }  
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
void Transmitir(unsigned char BufferT){
  while(TRMT==0);
  TXREG=BufferT;  
}
void putch(char data){
  while(TRMT==0);
  TXREG=data;
}
void interrupt ISR(void){
  TMR0IF=0;
  TMR0=3036;
  LATD0=LATD0^1;
  if(etimeout==1)   //Se cuenta cada segundo si esta habilitada la condición
    ctimeout++;     // de antibloqueo
  else
    ctimeout=0;     //Si no esta habilitada se reinicia la variable
  if(ctimeout>=2)   //Si la condición de antibloqueo excede dos cuentas
    etimeout=0;     //se coloca la habilitación del antibloqueo en 0
}
