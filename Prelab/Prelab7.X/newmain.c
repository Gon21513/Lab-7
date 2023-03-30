/* 
 * File:   newmain.c
 * Author: Luis Pedro Gonzalez
 *
 * Created on 23 de marzo de 2023, 04:57 PM
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


#define _XTAL_FREQ 8000000


////Rutina de interrupciones
void __interrupt() isr(void){
}

/////////////////////setup
void setup(void); //prototipo de setuo
void setupADC(void); //prototipo ADC


//////////main
void main(void){
    setup();//llamar a las configuraciones genreales
    setupADC();//LLamar a la configuracion del adc
    
}


//////////////setup general

void setup(void){
    // --------------- Definir como digitales --------------- 
    ANSELbits.ANS0 = 1;
    ANSELbits.ANS1 = 1;
    ANSELH = 0;
    
    // --------------- Configura puertos --------------- 
    //TRISA = 0; // Configura PORTA como salida
    TRISC = 0; // Configura PORTC como salida
    TRISD = 0; // Configura PORTD como salida
    TRISE = 0; // Configura PORTE como salida
    
    
    // --------------- limpiar puertos --------------- 
    PORTA = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    // --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b111 ; // establecerlo en 8 MHz
    OSCCONbits.SCS = 1; // utilizar oscilador intern
    
    // --------------- TMR0 --------------- 
    OPTION_REGbits.T0CS = 0; // utilizar el reloj interno (fosc/4)
    OPTION_REGbits.PSA = 0; // asignar el prescaler a Timer0
    OPTION_REGbits.PS2 = 1; // utilizar prescaler de 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;  
    TMR0 = 216; ///VALOR INICIAL DEL TMR0
    
    // --------------- Banderas e interrupciones --------------- 
    INTCONbits.T0IF = 0; // establece la bandera de la interrupcion del TMR0 apagada
    INTCONbits.T0IE = 1; // habilitar iinterrupcion del TMR0
    INTCONbits.GIE = 1; // habilitar interrupciones globales
    INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    PIE1bits.ADIE = 1; // habilitar interrupciones de ADC
    PIR1bits.ADIF = 0; // limpiar la bandera de interrupcion del ADC
}

// --------------- Setup del ADC --------------- 
void setupADC(void){
    // --------------- Configura el canal --------------- 
    ADCON0bits.CHS = 0b0000; // seleccionar AN0
    
            
    // --------------- Seleccion voltaje referencia --------------- 
    ADCON1bits.VCFG1 = 0; // Voltaje de referencia de 0V
    ADCON1bits.VCFG0 = 0; // Voltaje de referencia de 5V
            
    // --------------- Seleccion de reloj ---------------
    ADCON0bits.ADCS = 0b10; // Fosc/32
            
    // --------------- Asignar 8 bits, justificado izquierda ---------------
    ADCON1bits.ADFM = 0;        
            
    //--------------- Iniciar el ADC ---------------
    ADCON0bits.ADON = 1;  
    __delay_ms(1);
}