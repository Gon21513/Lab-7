/*
 * File:   manual.c
 * Author: lpgp3
 *
 * Created on 10 de abril de 2023, 09:06 PM
 */

#include "manual.h"
#include <xc.h>


#define tmr0_val 249 //interrupcion 0.015us
#define cont_pwm PORTCbits.RC3 
uint8_t cont_led; //variable de comparacion para se;al pwm
uint8_t pwm_led; //variable para almacernar valor del adc para pwm



void manualPWM_setup(void) {
    // Configurar el pin de salida para el PWM manual
    TRISCbits.TRISC3 = 0;
    
    // Configurar TMR0 y sus interrupciones 
    // --------------- TMR0 --------------- 
    OPTION_REGbits.T0CS = 0; // utilizar el reloj interno (fosc/4)
    OPTION_REGbits.PSA = 0; // asignar el prescaler a Timer0
    OPTION_REGbits.PS2 = 0; // utilizar prescaler de 2
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 0;  
    TMR0 = tmr0_val; ///VALOR INICIAL DEL TMR0
}

void manualPWM_ISR(void) {
        cont_led ++; //incrementar cintador del tmr0
        
        if (cont_led <= pwm_led){//comparar el valor del contador con el valor del potenciometro
            cont_pwm = 1; //si el valor del contador es menor o igual a 1 encide el puerto
        }
        else{
            cont_pwm = 0; // en cualquier otra situacion apagar el puerto
            
        }
        INTCONbits.T0IF = 0; //limpia bandera del tmr0
        TMR0 = tmr0_val;
}
