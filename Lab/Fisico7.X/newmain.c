/* 
 * File:   newmain.c
 * Author: Luis Pedro Gonzalez
 *
 * Created on 31 de marzo de 2023, 04:57 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>


#define _XTAL_FREQ 4000000  //frecuencia de 4MHZ

//////--------valores del potenciometro--------------
#define potmin 0//valor minimo del potenciometro 
#define potmax 255//valor maxiomo del potenciometro
#define pwmmin 100//valor minimo el pwm 0.4 ms para
//#define pwmmax 650//valor maximo para pwm 2.4 ms
#define pwmmax 600
//variables para controlar el ancho de pulso
unsigned int CCPRA = 0; //variable para el ccpr1
unsigned int CCPRB = 0; //variable para el ccpr2


/////////////////////setup
void setup(void); //prototipo de setuo
void setupADC(void); //prototipo ADC
void setupPWM(void); //prototipo del PWM


//------------mapeo de los valores -----------------
unsigned short cambiopwm(uint8_t valor, uint8_t POTMIN, uint8_t POTMAX,
        unsigned short PWMMIN, unsigned short PWMMAX);


////Rutina de interrupciones
void __interrupt() isr(void){
 ////interrupcion para primer potecniometro
    if (PIR1bits.ADIF){ //chequea interrupcion de adc
        if (ADCON0bits.CHS == 0b0000){ //revisa el canal 1 an0
            CCPRA = cambiopwm(ADRESH, potmin, potmax, pwmmin, pwmmax);//se mapean los valores 
            CCPR1L = (uint8_t)(CCPRA>>2);//asigna los 8 bits mas significativos a cpr1l
            CCP1CONbits.DC1B = CCPRA & 0b11; //asigna a dc1b los 2 bits menos significaticos
        }
        
        else if (ADCON0bits.CHS ==  0b0010){//chequea la interrupcion del adc
            CCPRB = cambiopwm(ADRESH, potmin, potmax, pwmmin, pwmmax);//se mapean los valores 
            CCPR2L = (uint8_t)(CCPRB>>2);//asigna los 8 bits mas significativos a cpr2l
            CCP2CONbits.DC2B0 = CCPRB & 0b01; //se le asigna el primer bit menos significativo
            CCP2CONbits.DC2B1 = CCPRB & 0b10; //se le asigna el segundo bit menos significativo
        }
        PIR1bits.ADIF = 0; //limpia la bandera del adc
    }
}

//////////main
void main(void){
    setup();//llamar a las configuraciones genreales
    setupADC();//LLamar a la configuracion del adc
    setupPWM();
        while (1){
        if (ADCON0bits.GO == 0) { // Chequea si el ADC est� encendido
            if (ADCON0bits.CHS == 0b0000) { // Chequea el canal 0
                ADCON0bits.CHS = 0b0010; // Cambia a canal 1// 0b0010
                
            } else if (ADCON0bits.CHS == 0b0010) { // Chequea el canal 1
                ADCON0bits.CHS = 0b0000; // Cambia a canal anal�gico 0
            }

            ADCON0bits.GO = 1; // Inicio de conversi�n
            __delay_us(20); // Delay despu�s de iniciar la conversi�n
                
            }
}
    return;
}


//////////////setup general

void setup(void){
    // --------------- Definir como digitales --------------- 
    ANSELH = 0; //puertos digitales 
    ANSELbits.ANS0 = 1; // ra0 como analogico
    ANSELbits.ANS2 = 1; //ra1 como analogico (era este)

    
    
    // --------------- Configura puertos --------------- 
    TRISAbits.TRISA0 = 1; //puerto A0 como entrada
    TRISAbits.TRISA2 = 1; //puerto A1 como entrada (era este)
    
    // --------------- limpiar puertos --------------- 
    PORTA = 0;
    PORTC = 0;
 
    
    // --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b0110 ; // establecerlo en 4 MHz
    OSCCONbits.SCS = 1; // utilizar oscilador intern
    
    // --------------- TMR0 --------------- 
//    OPTION_REGbits.T0CS = 0; // utilizar el reloj interno (fosc/4)
//    OPTION_REGbits.PSA = 0; // asignar el prescaler a Timer0
//    OPTION_REGbits.PS2 = 1; // utilizar prescaler de 256
//    OPTION_REGbits.PS1 = 1;
//    OPTION_REGbits.PS0 = 1;  
//    TMR0 = 216; ///VALOR INICIAL DEL TMR0
    
    // --------------- INTERRUPCIONES --------------- 
    //INTCONbits.T0IF = 0; // establece la bandera de la interrupcion del TMR0 apagada
    //INTCONbits.T0IE = 0; // habilitar iinterrupcion del TMR0
    INTCONbits.GIE = 1; // habilitar interrupciones globales
    INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    PIE1bits.ADIE = 1; // habilitar interrupciones de ADC
    PIR1bits.ADIF = 0; // limpiar la bandera de interrupcion del ADC
}

// --------------- Setup del ADC --------------- 
void setupADC(void){
    
    // --------------- Seleccion de reloj ---------------
    ADCON0bits.ADCS = 0b10; // Fosc/32
    
    // --------------- Seleccion voltaje referencia --------------- 
    ADCON1bits.VCFG1 = 0; // Voltaje de referencia de 0V
    ADCON1bits.VCFG0 = 0; // Voltaje de referencia de 5V
    
            
    // --------------- justificado izquierda ---------------
    ADCON1bits.ADFM = 0; 
    
    // --------------- canales --------------- 
    ADCON0bits.CHS = 0b0000; // seleccionar AN0
           
            
    //--------------- Iniciar el ADC ---------------
    ADCON0bits.ADON = 1;  //INICIA EL ADC
    __delay_ms(1);
}

//pr2,bits prescale y oscilador afectan al servo

//-------setup de PWM-------------------
void setupPWM(void){
  //--------------ccp1---------------    
    TRISCbits.TRISC1 = 1; //CCP1 como entrada
    TRISCbits.TRISC2 = 1; //CCP2 como entrada
    
    PR2 = 249 ;   //periodo de 4ms en el tmr2
    
    ////------------configuracion e ccp1------------------
    CCP1CON = 0; //APAGA CCP1 INICIALMENTE
    CCP2CON = 0; //APAGA CCP2 INICIALMENTE
    
    CCP1CONbits.P1M = 0; //modo de single output
    CCP1CONbits.CCP1M = 0b1100; //modo pwm para ccp1
    CCP2CONbits.CCP2M = 0b1100; //modo pwm para ccp2

    
    CCPR1L = 250>>2; //asiga 2 bits de 250 a ccpr1l
    CCP1CONbits.DC1B = 250 & 0b11;//asigna los bits menos sigificaticos del and a dc1b 
    CCPR2L = 250>>2;//los 2 bits desplazados se asignan a ccpr2l
    CCP2CONbits.DC2B0 = 250 & 0b01;//asigna el valor del and a dc2b0 
    CCP2CONbits.DC2B1 = 250 & 0b10; //asigna los bits a dc2b1
    //CCPR1L = 3; //valor inicla para que el servo inicie en 90 
    //CCP1CONbits.DC1B = 0b11; ///BITS menos significativos
    
    PIR1bits.TMR2IF = 0; //limpiar bandera del tmr2
    T2CONbits.T2CKPS = 0b11; //prescalr 16
    T2CONbits.TMR2ON = 1; //encender el tmr2
    
    while (!PIR1bits.TMR2IF);//ciclo de espera
    PIR1bits.TMR2IF = 0; //limpoar la bandera del tmr2
    
    TRISCbits.TRISC2 = 0; //habilitar salida en rc2
    TRISCbits.TRISC1 = 0; //habilitar salida en rc1
    return;

}

////funcion de mapeo e valores
unsigned short cambiopwm(uint8_t valor, uint8_t POTMIN, uint8_t POTMAX,
        unsigned short PWMMIN, unsigned short PWMMAX){
    return (unsigned short)(PWMMIN+((float)(PWMMAX-PWMMIN)/(POTMAX-POTMIN))
            *(valor-POTMIN));
}