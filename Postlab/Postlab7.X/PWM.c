/*
 * File:   PWM.c
 * Author: lpgp3
 *
 * Created on 1 de abril de 2023, 01:46 PM
 */
#include "PWM.h"
#include <xc.h>

// Definición de preescalador
#define PRESCALER 16
#define _XTAL_FREQ 4000000  //frecuencia de 4MHZ

void PWM_config(char canal, float periodo_ms) {
    PR2 = (unsigned char)((_XTAL_FREQ/1000 * periodo_ms)/(4 * PRESCALER) - 1);
    PIR1bits.TMR2IF = 0; // Limpiar bandera del TMR2
    T2CONbits.T2CKPS = 0b11; // Preescalar 16
    T2CONbits.TMR2ON = 1; // Encender el TMR2
    while (!PIR1bits.TMR2IF); // Ciclo de espera
    
    PIR1bits.TMR2IF = 0; // Limpiar la bandera del TMR2

    if (canal == 1) {
        TRISCbits.TRISC1 = 1; // CCP1 como entrada
        CCP1CON = 0; // Apagar CCP1 inicialmente
        CCP1CONbits.P1M = 0; // Modo de single output
        CCP1CONbits.CCP1M = 0b1100; // Modo PWM para CCP1
        TRISCbits.TRISC1 = 0; // Habilitar salida en RC1
    } else if (canal == 2) {
        TRISCbits.TRISC2 = 1; // CCP2 como entrada
        CCP2CON = 0; // Apagar CCP2 inicialmente
        CCP2CONbits.CCP2M = 0b1100; // Modo PWM para CCP2
        TRISCbits.TRISC2 = 0; // Habilitar salida en RC2
    }
    return;
}

void PWM_duty(char canal, float duty) {
    unsigned int duty_val = (unsigned int)((duty * (PR2+1) * 4) / 100.0);

    if (canal == 1) {
        CCPR1L = duty_val >> 2;
        CCP1CONbits.DC1B = duty_val & 0b11;
    } else if (canal == 2) {
        CCPR2L = duty_val >> 2;
        CCP2CONbits.DC2B0 = duty_val & 0b01;
        CCP2CONbits.DC2B1 = duty_val & 0b10;
    }
    return;
}